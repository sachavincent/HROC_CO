#include "scene.hpp"
#include "engine.hpp"
#include "bvh/boundingBoxObject.hpp"
#include "utils/jsonparser.hpp"
#include "frustum.hpp"

//#include <execution>
#include <string>
#include <map>

Scene::Scene(Engine *_engine) : engine(_engine), exposure(1.0), hierarchy(nullptr), nbObjects(0)
{
}

Scene::Scene(Engine *_engine, const std::string &_file) : engine(_engine), exposure(1.0), hierarchy(nullptr), nbObjects(0)
{
    const std::string path = Utils::workingDirectory() + "scenes/" + _file;
    std::ifstream file(path);
    if (!file.good())
        std::cerr << "Scene file '" << path << "' could not be found!" << std::endl;

    try
    {
        SceneData sceneData = JsonParser::parseFile(file);
        if (sceneData.updateFreeCam)
            sceneData.updateFreeCam.value()(_engine);
        if (sceneData.updateStaticCam)
            sceneData.updateStaticCam.value()(_engine);

        lights = sceneData.lights;
        objects = sceneData.objects;

        load();

        createBVH();
    }
    catch (const json::exception &e)
    {
        std::cerr << "Scene file '" << path << "' contains json error:\n\t" << e.what() << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Scene file '" << path << "' contains error:\n\t" << e.what() << std::endl;
    }
}

Scene::~Scene()
{
    objects.clear();
    lights.clear();
    boundingBoxes.clear();
    Object::flushCaches();

    delete hierarchy;

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &idVBO);
    glDeleteBuffers(1, &colorVBO);
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &inst);
    glDeleteBuffers(1, &cmd);
}

void Scene::requestBvhUpdate(int nbRequests)
{
    if (updateRequired == 0)
        updateRequired = nbRequests;
}

void Scene::stopBvhUpdate()
{
    updateRequired = 0;
}

void Scene::updateBvh()
{
    // if (updateRequired == 0)
    //   return;

    updateRequired--;
    std::cout << "Updating bvh." << std::endl;

    updateFrustum();

    glDepthMask(GL_TRUE);
    glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glDepthMask(GL_FALSE);

    double timerStart;
    timerStart = glfwGetTime();

    /*
     * 1 - Occlusion Map Rendering
     * in V indices of previously drawn objects
     * Using previous drawn objects indices V  to find effective occluders and give their indices O
     * Start frame use V with indices of all objects
     * => Early-Z to get indices O of on-screen objects using bounding box of nodes rendering
     * => Store OM (Depth map used later)
     * return list of indices of effectives occluders O
     */

    std::vector<unsigned int> V;
    for (auto obj : objects)
        V.push_back(obj->getId());

    // TODO: Early-Z with V (at first = everything) => returns effectiveOccluders
    // TODO store depth map
    std::vector<unsigned int> O = doEarlyZ(V);

    timers[0] = glfwGetTime() - timerStart; // Early-Z
    timerStart = glfwGetTime();

    /*
     * 2 - Extraction of Occludee Groups
     * in O indices of effectives occluders
     * Using previously found effectives occluders (O) to update BVHnodes visibility
     * and collect occluders of unknow visibility to add them in G (potential occluders)
     * => Traverse the BVH for each occluders using ExtractOccludees from BVH
     * Then perform View Frustum Culling on potential occludees G
     * return indices of culled potentential occludees G
     */
    std::vector<std::shared_ptr<BvhNode>> occluders;
    for (unsigned int o : O)
    {
        occluders.push_back(objects[o]->getBoundingBox()->getNode()); // Object idx => BvhNode
    }

    std::vector<std::shared_ptr<BvhNode>> G = hierarchy->extractOccludees(occluders);

    timers[1] = glfwGetTime() - timerStart; // Extract
    timerStart = glfwGetTime();

    const Frustum *f = engine->getStaticCamera()->getFrustum();
    std::vector<std::shared_ptr<BvhNode>> culledPotentialOccludees = f->ViewFrustumCulling(G);

    timers[2] = glfwGetTime() - timerStart; // VFC
    timerStart = glfwGetTime();

    /*
     * 3 - Batch Occlusion Test (Paper Original)
     * in OM occlusion map
     * Using the previously calculated occlusion map OM to start conservatives rays (from fragments) testing intersection with
     * bvhTree.
     * Explore bvh by starting from "leafest" (closest from leaf) node trigerring early-Z to add intersecting
     * objects to potentially visibles occludees U to be drawn.
     * return indices of potential visible occludees U
     */

    // TODO Raycast with aabb on GPU to extract U

    timers[3] = glfwGetTime() - timerStart; // Raycast
    timerStart = glfwGetTime();

    /*
     * 3 - Batch Occlusion Test (Our Implementation)
     *
     * return indices of potential visible occludees U
     */

    timers[4] = timerStart - glfwGetTime(); // Bb extract
    timerStart = glfwGetTime();

    glDisable(GL_DEPTH_TEST);
    simpleShader.start();
    simpleShader.loadMat4("view", engine->getStaticCamera()->getViewMatrix());
    simpleShader.loadMat4("projection", engine->getStaticCamera()->getProjectionMatrix());
    std::set<int> cache;
    std::vector<unsigned int> potentiallyVisibleOccludees = batchOcclusionTest(culledPotentialOccludees, cache);
    simpleShader.stop();
    glEnable(GL_DEPTH_TEST);

    timers[5] = glfwGetTime() - timerStart; // Batch occlusion Test
    timerStart = glfwGetTime();

    /*
     * 4 - Occludee Rendering
     * Make a new pass of early-Z on U and add passing objects to drawn objects D and draw them.
     * Merge D with potential occluders O to initialize V for the next frame.
     */
    // TODO Early Z on potentiallyVisibleOccludees to initialize drawnObjects

    glDepthMask(GL_TRUE);
    glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glDepthMask(GL_FALSE);
    std::sort(potentiallyVisibleOccludees.begin(), potentiallyVisibleOccludees.end(),
              [&](unsigned int a, unsigned int b)
              {
                  return glm::distance(engine->getStaticCamera()->getPosition(), objects[a]->getBoundingBox()->getCenter()) <
                         glm::distance(engine->getStaticCamera()->getPosition(), objects[b]->getBoundingBox()->getCenter());
              });
    std::vector<unsigned int> drawObjects = doEarlyZ(potentiallyVisibleOccludees);
    timers[6] = glfwGetTime() - timerStart; // Early Z on Rendering
    timerStart = glfwGetTime();

    timers[7] = glfwGetTime() - timerStart; // Draw objects
    timerStart = glfwGetTime();

    // TODO merge drawnObjects & effectiveOccluders to initialize previously drawn objects V
    objectVisibility = std::vector<bool>(objects.size(), false);
    for (unsigned int idxObj : drawObjects)
        objectVisibility[idxObj] = true;

    timers[8] = glfwGetTime() - timerStart; // Merge

    std::cout << "Bvh updated:\n\t1st Early-Z: " << O.size() << "\n\tExtractOccludees: " << G.size() << "\n\tVFC: " << culledPotentialOccludees.size() << "\n\tbatchOcclusionTest: " << potentiallyVisibleOccludees.size()
              << "\n\t2nd Early-Z: " << drawObjects.size() << std::endl;
}

//! Load the scene models on GPU before rendering
void Scene::load()
{
    std::vector<GLuint> ids;
    for (GLuint i = 0; i < objects.size(); i++)
        ids.push_back(i);
    std::vector<glm::vec3> colors;
    for (auto &o : objects)
        colors.push_back(o->getDiffuse());

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    int cmdCount = 0;
    cmds = MeshHandler::getSingleton()->getCmds(objects, &cmdCount);
    nbObjects = cmdCount;
    MeshHandler::getSingleton()->getBuffers(objects, vertices, indices);

    glCreateVertexArrays(1, &vao);
    glCreateBuffers(1, &vbo);
    glCreateBuffers(1, &idVBO);
    glCreateBuffers(1, &colorVBO);
    glCreateBuffers(1, &ebo);
    glCreateBuffers(1, &inst);
    glCreateBuffers(1, &cmd);

    glEnableVertexArrayAttrib(vao, 0);
    glEnableVertexArrayAttrib(vao, 1);
    glEnableVertexArrayAttrib(vao, 2);
    glEnableVertexArrayAttrib(vao, 3);
    glEnableVertexArrayAttrib(vao, 4);
    glEnableVertexArrayAttrib(vao, 5);
    glEnableVertexArrayAttrib(vao, 6);
    glEnableVertexArrayAttrib(vao, 7);
    glEnableVertexArrayAttrib(vao, 8);
    glNamedBufferStorage(vbo, std::size(vertices) * sizeof(Vertex), vertices.data(), 0);
    glNamedBufferStorage(ebo, std::size(indices) * sizeof(GLuint), indices.data(), 0);
    glNamedBufferStorage(inst, std::size(models) * sizeof(glm::mat4), models.data(), 0);

    glNamedBufferStorage(idVBO, std::size(ids) * sizeof(GLuint), ids.data(), 0);
    glNamedBufferStorage(colorVBO, std::size(colors) * sizeof(glm::vec3), colors.data(), 0);

    // glNamedBufferStorage(cmd, nbObjects * sizeof(DrawElementsCommand), cmds, 0);
    glNamedBufferData(cmd, cmdCount * sizeof(DrawElementsCommand), cmds, GL_DYNAMIC_DRAW);
    glVertexArrayElementBuffer(vao, ebo);                      // link vao to ebo
    glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Vertex)); // vbo bound to location 0 of vao

    glVertexArrayAttribBinding(vao, 0, 0);
    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Position));
    glVertexArrayAttribBinding(vao, 1, 0);
    glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Normal));
    glVertexArrayAttribBinding(vao, 2, 0);
    glVertexArrayAttribFormat(vao, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, TexCoord));

    glVertexArrayAttribBinding(vao, 3, 1);
    glVertexArrayAttribFormat(vao, 3, 4, GL_FLOAT, GL_FALSE, 0);

    glVertexArrayAttribBinding(vao, 4, 1);
    glVertexArrayAttribFormat(vao, 4, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4);
    glVertexArrayAttribBinding(vao, 5, 1);
    glVertexArrayAttribFormat(vao, 5, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 8);
    glVertexArrayAttribBinding(vao, 6, 1);
    glVertexArrayAttribFormat(vao, 6, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 12);
    glVertexArrayVertexBuffer(vao, 1, inst, 0, sizeof(glm::mat4));
    glVertexArrayBindingDivisor(vao, 1, 1);

    glVertexArrayAttribBinding(vao, 7, 2);
    glVertexArrayAttribIFormat(vao, 7, 1, GL_UNSIGNED_INT, 0);
    glVertexArrayVertexBuffer(vao, 2, idVBO, 0, sizeof(GLuint));
    glVertexArrayBindingDivisor(vao, 2, 1);

    glVertexArrayAttribBinding(vao, 8, 3);
    glVertexArrayAttribFormat(vao, 8, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayVertexBuffer(vao, 3, colorVBO, 0, sizeof(glm::vec3));
    glVertexArrayBindingDivisor(vao, 3, 1);

    sh = {"shaders/default.vert", "shaders/simple.frag"};
    simpleShader = {"shaders/boundingBox.vert", "shaders/query.frag"};
    earlyZShader = {"shaders/early.vert", "shaders/early.frag"};
    bbShader = {"shaders/boundingBox.vert", "shaders/boundingBox.frag"};
    frustumShader = {"shaders/frustum.vert", "shaders/frustum.frag"};

    createFrustum();
    setupEarlyZCommand();
    updateRequired = true;
}

void Scene::createBVH()
{
    std::vector<std::shared_ptr<BoundingBox>> bbs;
    const std::vector<std::shared_ptr<Object>> &_objects = getObjects();
    for (auto obj : _objects)
    {
        auto newBoundingBox = std::make_shared<AxisBoundingBox>(obj);
        obj->setBoundingBox(newBoundingBox);
        if (obj)
            bbs.push_back(obj->getBoundingBox());
    }

    hierarchy = new BvhTree(bbs);

    boundingBoxes = hierarchy->getDebugData();
}
//! Render all objects of scene
void Scene::renderObjects()
{
    // set shaders params
    sh.start();

    sh.loadMat4("view", getCamera()->getViewMatrix());
    sh.loadMat4("projection", getCamera()->getProjectionMatrix());
    sh.loadFloat("exposure", exposure);
    sh.loadVec3("viewPos", getCamera()->getPosition());

    for (uint32_t i = 0; i < std::min(lights.size(), (size_t)MAXLIGHTS); i++)
    {
        sh.loadBool("lights[" + std::to_string(i) + "].enabled", 1);

        sh.loadVec3("lights[" + std::to_string(i) + "].position",
                    lights[i]->getPosition());

        sh.loadVec3("lights[" + std::to_string(i) + "].color",
                    lights[i]->getColor());
        sh.loadVec3("lights[" + std::to_string(i) + "].attenuation",
                    lights[i]->getAttenuation());
    }
    if (lights.size() < MAXLIGHTS)
    {
        for (size_t i = lights.size(); i < MAXLIGHTS; i++)
        {
            sh.loadBool("lights[" + std::to_string(i) + "].enabled", 0);
        }
    }
    bool debugVisibility = true;
    sh.loadBool("debugVisibility", debugVisibility);

    // TODO: 1 draw of visible objects + 1 draw of invisible objects (if debugVisibility == true)

    // draw objects if gui enables it
    if (engine->getUi().getObjectsVisMode())
    {

        glBindVertexArray(vao);
        std::vector<std::shared_ptr<Object>> listObjVisible;
        std::vector<std::shared_ptr<Object>> listObjInvisible;
        for (size_t i = 0; i < objectVisibility.size(); i++)
        {
            if (objectVisibility[i] == 1)
                listObjVisible.push_back(objects[i]);
            else if (debugVisibility)
                listObjInvisible.push_back(objects[i]);
        }
        if (listObjVisible.empty())
            return;

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LESS);
        int cmdCount;
        if (debugVisibility && !listObjInvisible.empty())
        {
            sh.loadBool("visible", false);

            auto newCmdsInvis = MeshHandler::getSingleton()->getCmdsForSubset(listObjInvisible, &cmdCount);
            glNamedBufferData(cmd, cmdCount * sizeof(DrawElementsCommand), newCmdsInvis, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, cmd);
            glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid *)0, cmdCount, 0);
        }

        if (debugVisibility)
            sh.loadBool("visible", true);
        auto newCmds = MeshHandler::getSingleton()->getCmdsForSubset(listObjVisible, &cmdCount);

        glNamedBufferData(cmd, cmdCount * sizeof(DrawElementsCommand), newCmds, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, cmd);
        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid *)0, cmdCount, 0);

        glBindVertexArray(0);
    }
    // unload shader
    sh.stop();
}

//! Render all objects of given vector
void Scene::renderObjects(std::vector<Object *> &_objects)
{
    // set shaders params
    sh.start();

    sh.loadMat4("view", getCamera()->getViewMatrix());
    sh.loadMat4("projection", getCamera()->getProjectionMatrix());
    sh.loadFloat("exposure", exposure);
    sh.loadVec3("viewPos", getCamera()->getPosition());

    for (uint32_t i = 0; i < std::min(lights.size(), (size_t)MAXLIGHTS); i++)
    {
        sh.loadBool("lights[" + std::to_string(i) + "].enabled", 1);

        sh.loadVec3("lights[" + std::to_string(i) + "].position",
                    lights[i]->getPosition());

        sh.loadVec3("lights[" + std::to_string(i) + "].color",
                    lights[i]->getColor());
        sh.loadVec3("lights[" + std::to_string(i) + "].attenuation",
                    lights[i]->getAttenuation());
    }
    if (lights.size() < MAXLIGHTS)
    {
        for (size_t i = lights.size(); i < MAXLIGHTS; i++)
        {
            sh.loadBool("lights[" + std::to_string(i) + "].enabled", 0);
        }
    }

    // draw objects if gui enables it
    if (engine->getUi().getObjectsVisMode())
    {
        for (size_t i = 0; i < objects.size(); i++)
        {
            // _objects[i]->draw(sh);
        }
    }

    // unload shader
    sh.stop();
}

//! Render one object
void Scene::renderObject(Object &obj)
{
    // set shaders params
    sh.start();

    sh.loadMat4("view", getCamera()->getViewMatrix());
    sh.loadMat4("projection", getCamera()->getProjectionMatrix());
    sh.loadFloat("exposure", exposure);
    sh.loadVec3("viewPos", getCamera()->getPosition());

    for (uint32_t i = 0; i < std::min(lights.size(), (size_t)MAXLIGHTS); i++)
    {
        sh.loadBool("lights[" + std::to_string(i) + "].enabled", 1);

        sh.loadVec3("lights[" + std::to_string(i) + "].position",
                    lights[i]->getPosition());

        sh.loadVec3("lights[" + std::to_string(i) + "].color",
                    lights[i]->getColor());
        sh.loadVec3("lights[" + std::to_string(i) + "].attenuation",
                    lights[i]->getAttenuation());
    }
    if (lights.size() < MAXLIGHTS)
    {
        for (size_t i = lights.size(); i < MAXLIGHTS; i++)
        {
            sh.loadBool("lights[" + std::to_string(i) + "].enabled", 0);
        }
    }

    // obj.draw(sh);
    //  unload shader
    sh.stop();
}

void Scene::renderBoundingBoxes()
{
    if (!hierarchy)
        return;

    bbShader.start();

    bbShader.loadMat4("view", getCamera()->getViewMatrix());
    bbShader.loadMat4("projection", getCamera()->getProjectionMatrix());

    int visMode = engine->getUi().getBboxVisMode();
    // no bbox vis mode
    if (visMode == -1)
        return;
    BoundingBoxObject::bind();
    int bboxLevel;
    int maxBboxLevel = 0;
    for (auto entry : boundingBoxes)
    {
        int numBB = 0;
        bboxLevel = entry.first;
        maxBboxLevel = std::max(maxBboxLevel, bboxLevel);
        auto bboxs = entry.second;
        if (visMode == 0 || bboxLevel == visMode)
        {
            for (auto bbox : bboxs)
                bbox.get()->draw(bbShader, numBB++);
        }
    }
    BoundingBoxObject::unbind();
    engine->getUi().setBboxMaxLevel(maxBboxLevel);
    bbShader.stop();
}

void Scene::createFrustum()
{
    glm::mat4 view, proj;
    Camera *staticCamera = engine->getStaticCamera();
    proj = staticCamera->getProjectionMatrix();
    view = staticCamera->getViewMatrix();
    std::array<glm::vec3, 8> _cameraFrustumCornerVertices{
        {
            {-1.0f, -1.0f, 1.0f},
            {1.0f, -1.0f, 1.0f},
            {1.0f, 1.0f, 1.0f},
            {-1.0f, 1.0f, 1.0f},
            {-1.0f, -1.0f, -1.0f},
            {1.0f, -1.0f, -1.0f},
            {1.0f, 1.0f, -1.0f},
            {-1.0f, 1.0f, -1.0f},
        }};

    const auto inv = glm::inverse(proj * view);
    std::array<glm::vec3, 8> _frustumVertices;

    std::transform(
        _cameraFrustumCornerVertices.begin(),
        _cameraFrustumCornerVertices.end(),
        _frustumVertices.begin(),
        [&](glm::vec3 p)
        {
            auto v = inv * glm::vec4(p, 1.0f);
            v.z = v.z;
            return glm::vec3(v) / v.w;
        });
    glm::vec3 *vertices = _frustumVertices.data();
    staticFrustumObject = new FrustumObject("debugFrustum", vertices, "Frustum");
}

void Scene::updateFrustum()
{
    glm::mat4 view, proj;
    Camera *staticCamera = engine->getStaticCamera();
    proj = staticCamera->getProjectionMatrix();
    view = staticCamera->getViewMatrix();
    std::array<glm::vec3, 8> _cameraFrustumCornerVertices{
        {
            {-1.0f, -1.0f, 1.0f},
            {1.0f, -1.0f, 1.0f},
            {1.0f, 1.0f, 1.0f},
            {-1.0f, 1.0f, 1.0f},
            {-1.0f, -1.0f, -1.0f},
            {1.0f, -1.0f, -1.0f},
            {1.0f, 1.0f, -1.0f},
            {-1.0f, 1.0f, -1.0f},
        }};

    const auto inv = glm::inverse(proj * view);
    std::array<glm::vec3, 8> _frustumVertices;

    std::transform(
        _cameraFrustumCornerVertices.begin(),
        _cameraFrustumCornerVertices.end(),
        _frustumVertices.begin(),
        [&](glm::vec3 p)
        {
            auto v = inv * glm::vec4(p, 1.0f);
            return glm::vec3(v) / v.w;
        });
    glm::vec3 *vertices = _frustumVertices.data();
    staticFrustumObject->adjustVertexData(vertices);
}

void Scene::renderFrustum(bool outline)
{
    bool frustumVisMode;
    outline ? frustumVisMode = engine->getUi().getFrustumVisMode() : frustumVisMode = engine->getUi().getFrustumOutlineVisMode();
    if (!frustumVisMode || engine->getCurrentCameraType() == CameraType::STATIC)
        return;
    FrustumObject::bind();
    frustumShader.start();
    frustumShader.loadBool("outline", outline);
    frustumShader.loadMat4("view", getCamera()->getViewMatrix());
    frustumShader.loadMat4("projection", getCamera()->getProjectionMatrix());

    outline ? staticFrustumObject->drawOutline() : staticFrustumObject->draw();

    FrustumObject::unbind();
    frustumShader.stop();
}

//! Add an object to scene
Scene &Scene::addObject(std::shared_ptr<Object> _object)
{
    models.push_back(_object->getTransformationMatrix());
    objects.push_back(_object);
    return *this;
}

//! Add a light to the scene
Scene &Scene::addLight(std::shared_ptr<Light> _light)
{
    lights.push_back(_light);
    return *this;
}

Camera *Scene::getCamera() { return engine->getCurrentCamera(); }

std::vector<unsigned int> Scene::batchOcclusionTest(std::vector<std::shared_ptr<BvhNode>> occludeeGroups, std::set<int> &cache)
{
    unsigned int THRESHOLD = 10; // Min samples

    /*std::sort(occludeeGroups.begin(), occludeeGroups.end(),
              [&](unsigned int a, unsigned int b)
              {
                  return glm::distance(staticCam->getPosition(), objects[a]->getBoundingBox()->getCenter()) <
                         glm::distance(staticCam->getPosition(), objects[b]->getBoundingBox()->getCenter());
              });*/

    std::vector<unsigned int> potentiallyVisibleOccludees;
    std::vector<std::shared_ptr<BvhNode>> nextOccludeeGroups;
    const size_t nbQueries = occludeeGroups.size();
    GLuint *queries = new GLuint[nbQueries];
    glGenQueries(nbQueries, queries);

    unsigned int i = 0;
    for (std::shared_ptr<BvhNode> node : occludeeGroups)
    {
        cache.insert(node->getId());
        glBeginQuery(GL_SAMPLES_PASSED, queries[i++]);
        node->getBoundingBox()->getWireframe()->drawQuery(simpleShader);
        glEndQuery(GL_SAMPLES_PASSED);
    }

    for (unsigned int j = 0; j < i; j++)
    {
        GLuint nbSamples;
        GLint available = GL_FALSE;
        while (available == GL_FALSE)
            glGetQueryObjectiv(queries[j], GL_QUERY_RESULT_AVAILABLE, &available);

        glGetQueryObjectuiv(queries[j], GL_QUERY_RESULT, &nbSamples);

        if (nbSamples > THRESHOLD)
        {
            if (occludeeGroups[j]->isLeaf())
                potentiallyVisibleOccludees.push_back(occludeeGroups[j]->getBoundingBox()->getObject()->getId());
            else
            {
                if (occludeeGroups[j]->hasLeftChild() && cache.count(occludeeGroups[j]->getLeftChild()->getId()) == 0)
                    nextOccludeeGroups.push_back(occludeeGroups[j]->getLeftChild());
                if (occludeeGroups[j]->hasRightChild() && cache.count(occludeeGroups[j]->getRightChild()->getId()) == 0)
                    nextOccludeeGroups.push_back(occludeeGroups[j]->getRightChild());
            }
        }
    }

    glDeleteQueries(nbQueries, queries);

    delete queries;

    if (!nextOccludeeGroups.empty())
    {
        std::vector<unsigned int> visibleOccludees = batchOcclusionTest(nextOccludeeGroups, cache);
        potentiallyVisibleOccludees.insert(potentiallyVisibleOccludees.end(), visibleOccludees.begin(), visibleOccludees.end());
    }
    return potentiallyVisibleOccludees;
}

std::vector<unsigned int> Scene::doEarlyZ(std::vector<unsigned int> _idObjects)
{
    if (_idObjects.empty())
        return std::vector<unsigned int>();

    Camera *staticCam = engine->getStaticCamera();

    defaultVisibility = new int[_idObjects.size()];

    for (int i = 0; i < _idObjects.size(); ++i)
    {
        defaultVisibility[i] = 0;
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * _idObjects.size(), defaultVisibility, GL_DYNAMIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    int *_visibility = new int[_idObjects.size()];

    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);

    earlyZShader.start();
    earlyZShader.loadMat4("view", staticCam->getViewMatrix());
    earlyZShader.loadMat4("projection", staticCam->getProjectionMatrix());
    glBindVertexArray(vao);

    int nbCmds = 0;
    std::vector<std::shared_ptr<Object>> obj;
    for (auto idxObj : _idObjects)
        obj.push_back(objects[idxObj]);
    DrawElementsCommand *earlyZcmds = MeshHandler::getSingleton()->getCmdsForSubset(obj, &nbCmds);
    if (!earlyZcmds)
        return std::vector<unsigned int>();

    glNamedBufferData(cmd, nbCmds * sizeof(DrawElementsCommand), earlyZcmds, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, cmd);
    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid *)0, size_t(nbCmds), 0);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(int) * _idObjects.size(), _visibility);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glBindVertexArray(0);

    earlyZShader.stop();

    glDepthFunc(GL_EQUAL);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_FALSE);

    std::vector<unsigned int> test(_visibility, _visibility + _idObjects.size());
    std::vector<unsigned int> visibleObjects;
    delete earlyZcmds;

    for (size_t i = 0; i < _idObjects.size(); i++)
    {
        if (_visibility[i] == 1)
            visibleObjects.push_back(_idObjects[i]);
    }
    delete _visibility;
    return visibleObjects;
}
