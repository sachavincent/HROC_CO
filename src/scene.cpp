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
    // TODO:
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

void Scene::updateBvh()
{
    double start, end;
    /*
     * 1 - Occlusion Map Rendering
     * in V indices of previously drawn objects
     * Using previous drawn objects indices V  to find effective occluders and give their indices O
     * Start frame use V with indices of all objects
     * => Early-Z to get indices O of on-screen objects using bounding box of nodes rendering
     * => Store OM (Depth map used later)
     * return list of indices of effectives occluders O
     */

    start = glfwGetTime();
    std::vector<BvhNode *> effectiveOccluders;
    // TODO: Early-Z with V (at first = everything) => returns effectiveOccluders
    // TODO store depth map
    end = glfwGetTime();

    timers[0] = end - start;
    start = glfwGetTime();

    /*
     * 2 - Extraction of Occludee Groups
     * in O indices of effectives occluders
     * Using previously found effectives occluders (O) to update BVHnodes visibility
     * and collect occluders of unknow visibility to add them in G (potential occluders)
     * => Traverse the BVH for each occluders using ExtractOccludees from BVH
     * Then perform View Frustum Culling on potential occludees G
     * return indices of culled potentential occludees G
     */

    std::vector<BvhNode *> potentialOccluders = hierarchy->extractOccludees(effectiveOccluders); // = G
    end = glfwGetTime();

    timers[1] = end - start;
    start = glfwGetTime();

    const Frustum *f = getCamera()->getFrustum();
    std::vector<std::shared_ptr<BvhNode>> culledPotentialOccludees = f->ViewFrustumCulling(potentialOccluders);
    end = glfwGetTime();

    timers[2] = end - start;
    start = glfwGetTime();

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
    end = glfwGetTime();

    timers[3] = end - start;
    start = glfwGetTime();

    /*
     * 3 - Batch Occlusion Test (Our Implementation)
     *
     * return indices of potential visible occludees U
     */
    end = glfwGetTime();

    timers[4] = end - start;
    start = glfwGetTime();

    std::vector<std::shared_ptr<BvhNode>> potentiallyVisibleOccludees = batchOcclusionTest(culledPotentialOccludees);
    end = glfwGetTime();

    timers[5] = end - start;
    start = glfwGetTime();

    /*
     * 4 - Occludee Rendering
     * Make a new pass of early-Z on U and add passing objects to drawn objects D and draw them.
     * Merge D with potential occluders O to initialize V for the next frame.
     */
    // TODO Early Z on potentiallyVisibleOccludees to initialize drawnObjects

    end = glfwGetTime();
    timers[6] = end - start;
    start = glfwGetTime();
    // doEarlyZ();
    std::vector<Object *> drawnObjects;

    renderObjects(drawnObjects);
    end = glfwGetTime();

    timers[7] = end - start;
    start = glfwGetTime();
    // TODO merge drawnObjects & effectiveOccluders to initialize previously drawn objects V
    end = glfwGetTime();

    timers[8] = end - start;
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
    simpleShader = {"shaders/default.vert", "shaders/simple.frag"};
    earlyZShader = {"shaders/early.vert", "shaders/early.frag"};
    bbShader = {"shaders/boundingBox.vert", "shaders/boundingBox.frag"};
    frustumShader = {"shaders/frustum.vert", "shaders/frustum.frag"};

    setupEarlyZCommand();
}

void Scene::createBVH()
{
    std::vector<std::shared_ptr<BoundingBox>> bbs;
    const std::vector<std::shared_ptr<Object>> &_objects = getObjects();
    for (auto obj : _objects)
    {
        Object *bbo = obj.get();
        auto newBoundingBox = std::make_shared<AxisBoundingBox>(*bbo);
        bbo->setBoundingBox(newBoundingBox);
        if (bbo)
            bbs.push_back(bbo->getBoundingBox());
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

    // draw objects if gui enables it
    if (engine->getUi().getObjectsVisMode())
    {
        glBindVertexArray(vao);
        glNamedBufferData(cmd, nbObjects * sizeof(DrawElementsCommand), cmds, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, cmd);
        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid *)0, nbObjects, 0);

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
        bboxLevel = entry.first + 1;
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
void Scene::createFrustum(){
    glm::mat4 view, proj;
    Camera * staticCamera = engine->getStaticCamera();
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
        }
    );
    glm::vec3 * vertices = _frustumVertices.data();
    staticFrustumObject = new FrustumObject("debugFrustum",vertices,"Frustum");
}
void Scene::updateFrustum(){
    glm::mat4 view, proj;
    Camera * staticCamera = engine->getStaticCamera();
    proj = staticCamera->getProjectionMatrix();
    view = staticCamera->getViewMatrix();
    std::array<glm::vec3, 8> _cameraFrustumCornerVertices{
    {
        { -1.0f, -1.0f, 1.0f }, { 1.0f, -1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { -1.0f, 1.0f, 1.0f },
        { -1.0f, -1.0f, -1.0f }, { 1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f, -1.0f }, { -1.0f, 1.0f, -1.0f },
    }
    };


    const auto inv = glm::inverse(proj * view);
    std::array<glm::vec3, 8> _frustumVertices;

    std::transform(
        _cameraFrustumCornerVertices.begin(),
        _cameraFrustumCornerVertices.end(),
        _frustumVertices.begin(),
        [&](glm::vec3 p) {
            auto v =  inv * glm::vec4(p, 1.0f) ;
            v.z = v.z;
            return glm::vec3(v) / v.w;
        }
    );
    glm::vec3 * vertices = _frustumVertices.data();
    staticFrustumObject->adjustVertexData(vertices);

}
void Scene::renderFrustum(){    
    bool frustumVisMode = engine->getUi().getFrustumVisMode();
    if (!frustumVisMode)
        return;
    FrustumObject::bind();    
    frustumShader.start();
    frustumShader.loadMat4("view", getCamera()->getViewMatrix());
    frustumShader.loadMat4("projection", getCamera()->getProjectionMatrix());
    staticFrustumObject->draw();
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

std::vector<std::shared_ptr<BvhNode>> Scene::batchOcclusionTest(std::vector<std::shared_ptr<BvhNode>> &occludeeGroups)
{
    Camera *staticCam = engine->getStaticCamera();
    simpleShader.start();
    simpleShader.loadMat4("view", staticCam->getViewMatrix());
    simpleShader.loadMat4("projection", staticCam->getProjectionMatrix());

    unsigned int THRESHOLD = 10; // Min samples

    std::sort(/*std::execution::par_unseq, */ occludeeGroups.begin(), occludeeGroups.end(),
              [staticCam](std::shared_ptr<BvhNode> a, std::shared_ptr<BvhNode> b)
              {
                  return glm::distance(staticCam->getPosition(), a->getBoundingBox()->getCenter()) < glm::distance(staticCam->getPosition(), b->getBoundingBox()->getCenter());
              });

    std::vector<std::shared_ptr<BvhNode>> potentiallyVisibleOccludees;
    const size_t nbQueries = occludeeGroups.size();
    GLuint *queries = new GLuint[nbQueries];
    glGenQueries(nbQueries, queries);

    unsigned int i = 0;
    for (std::shared_ptr<BvhNode> bb : occludeeGroups)
    {
        glBeginQuery(GL_SAMPLES_PASSED, queries[i++]);
        bb->getBoundingBox()->getWireframe()->drawQuery(simpleShader);
        glEndQuery(GL_SAMPLES_PASSED);
    }

    for (unsigned int j = 0; j < i; j++)
    {
        unsigned int nbSamples;
        GLint available = GL_FALSE;
        while (available == GL_FALSE)
            glGetQueryObjectiv(queries[j], GL_QUERY_RESULT_AVAILABLE, &available);

        glGetQueryObjectuiv(queries[j], GL_QUERY_RESULT, &nbSamples);
        if (nbSamples > THRESHOLD)
            potentiallyVisibleOccludees.push_back(occludeeGroups[j]);
    }

    glDeleteQueries(nbQueries, queries);

    delete queries;

    simpleShader.stop();

    return potentiallyVisibleOccludees;
}

void Scene::doEarlyZ(std::vector<std::shared_ptr<Object>> _objects)
{
    Camera *staticCam = getCamera();

    std::sort(/*std::execution::par_unseq, */ _objects.begin(), _objects.end(), [staticCam](std::shared_ptr<Object> a, std::shared_ptr<Object> b)
              { return glm::distance(staticCam->getPosition(), a.get()->getPosition()) < glm::distance(staticCam->getPosition(), b.get()->getPosition()); });

    // Camera *staticCam = engine->getStaticCamera();
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * objects.size(), visibility, GL_DYNAMIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    int *_visibility = new int[objects.size()];
    /*    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(int) * objects.size(), _visibility);
        std::vector<int> valuesQ(_visibility, _visibility + objects.size());
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);*/
    // glEnable(GL_DEPTH_TEST);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);

    earlyZShader.start();
    earlyZShader.loadMat4("view", staticCam->getViewMatrix());
    earlyZShader.loadMat4("projection", staticCam->getProjectionMatrix());
    glBindVertexArray(vao);

    int nbCmds = 0;
    DrawElementsCommand *earlyZcmds = MeshHandler::getSingleton()->getCmdsForSubset(_objects, &nbCmds);

    glNamedBufferData(cmd, nbCmds * sizeof(DrawElementsCommand), earlyZcmds, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, cmd);
    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid *)0, size_t(nbCmds), 0);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(int) * objects.size(), _visibility);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glBindVertexArray(0);
    earlyZShader.stop();
    std::vector<int> values(_visibility, _visibility + objects.size());

    glDepthFunc(GL_EQUAL);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_FALSE);

    // glDisable(GL_DEPTH_TEST);
    delete _visibility;
    delete earlyZcmds;
}
