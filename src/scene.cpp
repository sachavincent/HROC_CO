#include "scene.hpp"
#include "engine.hpp"
#include "bvh/boundingBoxObject.hpp"
#include "utils/jsonparser.hpp"
#include "frustum.hpp"

#include <execution>
#include <string>
#include <map>

Scene::Scene(Engine *_engine) : engine(_engine), exposure(1.0), hierarchy(nullptr)
{
}

Scene::Scene(Engine *_engine, const std::string &_file) : engine(_engine), exposure(1.0), hierarchy(nullptr)
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
}

void Scene::updateBvh()
{
    /*
    * 1 - Occlusion Map Rendering
    * in V indices of previously drawn objects  
    * Using previous drawn objects indices V  to find effective occluders and give their indices O
    * Start frame use V with indices of all objects 
    * => Early-Z to get indices O of on-screen objects using bounding box of nodes rendering
    * => Store OM (Depth map used later) 
    * return list of indices of effectives occluders O
    */
    
    std::vector<BvhNode *> effectiveOccluders;
    // TODO: Early-Z with V (at first = everything) => returns effectiveOccluders
    // TODO store depth map 

   
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

    const Frustum *f = getCamera()->getFrustum();
    const std::vector<BvhNode *> culledPotentialOccludees = f->ViewFrustumCulling(potentialOccluders);


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


    /*
    * 3 - Batch Occlusion Test (Our Implementation) 
    *
    * return indices of potential visible occludees U
    */
    //TODO extract list of boundig boxes from G to initialize occludeeGroups_boundingBoxes
    std::vector<BoundingBox*> occludeeGroups_boundingBoxes;
    std::vector<BoundingBox *> potentiallyVisibleOccludees = batchOcclusionTest(occludeeGroups_boundingBoxes);


    /*
    * 4 - Occludee Rendering
    * Make a new pass of early-Z on U and add passing objects to drawn objects D and draw them.
    * Merge D with potential occluders O to initialize V for the next frame. 
    */
    //TODO Early Z on potentiallyVisibleOccludees to initialize drawnObjects
    std::vector<Object *> drawnObjects;

    renderObjects(drawnObjects);
    //TODO merge drawnObjects & effectiveOccluders to initialize previously drawn objects V 
    
}

//! Load the scene models on GPU before rendering
void Scene::load()
{
    // load models
    for (size_t i = 0; i < objects.size(); i++)
    {
        objects[i]->load();
    }
    // load shader
    sh = {"shaders/default.vert", "shaders/phong.frag"};
    simpleShader = {"shaders/default.vert", "shaders/simple.frag"};
    earlyZShader = {"shaders/early.vert", GL_VERTEX_SHADER};
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
        newBoundingBox.get()->getWireframe()->load();
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
    // depth parameter only for bboxes
    sh.loadInt("numBB", -1);

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
            objects[i]->draw(sh);
        }
    }

    // unload shader
    sh.stop();
}

//! Render all objects of given vector
void Scene::renderObjects(std::vector<Object*>& vector)
{
    // set shaders params
    sh.start();

    sh.loadMat4("view", getCamera()->getViewMatrix());
    sh.loadMat4("projection", getCamera()->getProjectionMatrix());
    sh.loadFloat("exposure", exposure);
    sh.loadVec3("viewPos", getCamera()->getPosition());
    // depth parameter only for bboxes
    sh.loadInt("numBB", -1);

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
            vector[i]->draw(sh);
        }
    }

    // unload shader
    sh.stop();
}

//!Render one object 
void Scene::renderObject(Object& obj) 
{
// set shaders params
    sh.start();

    sh.loadMat4("view", getCamera()->getViewMatrix());
    sh.loadMat4("projection", getCamera()->getProjectionMatrix());
    sh.loadFloat("exposure", exposure);
    sh.loadVec3("viewPos", getCamera()->getPosition());
    // depth parameter only for bboxes
    sh.loadInt("numBB", -1);

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

    
    obj.draw(sh);

    // unload shader
    sh.stop();
}

void Scene::renderBoundingBoxes()
{
    if (!hierarchy)
        return;

    int visMode = engine->getUi().getBboxVisMode();
    // no bbox vis mode
    if (visMode == -1)
        return;

    sh.start();
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
                bbox.get()->draw(sh, numBB++);
        }
    }
    engine->getUi().setBboxMaxLevel(maxBboxLevel);
    sh.stop();
}

//! Add an object to scene
Scene &Scene::addObject(std::shared_ptr<Object> _object)
{
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

std::vector<BoundingBox *> Scene::batchOcclusionTest(std::vector<BoundingBox *> &occludeeGroups)
{
    Camera *staticCam = engine->getStaticCamera();
    simpleShader.start();
    simpleShader.loadMat4("view", staticCam->getViewMatrix());
    simpleShader.loadMat4("projection", staticCam->getProjectionMatrix());

    unsigned int THRESHOLD = 10; // Min samples

    std::sort(std::execution::par_unseq, occludeeGroups.begin(), occludeeGroups.end(), [staticCam](BoundingBox *a, BoundingBox *b)
              { return glm::distance(staticCam->getPosition(), a->getCenter()) < glm::distance(staticCam->getPosition(), b->getCenter()); });

    std::vector<BoundingBox *> potentiallyVisibleOccludees;
    const size_t nbQueries = occludeeGroups.size();
    GLuint *queries = new GLuint[nbQueries];
    glGenQueries(nbQueries, queries);

    unsigned int i = 0;
    for (BoundingBox *bb : occludeeGroups)
    {
        glBeginQuery(GL_SAMPLES_PASSED, queries[i++]);
        bb->getWireframe()->drawQuery(simpleShader);
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
    Camera *staticCam = engine->getStaticCamera();
    std::sort(std::execution::par_unseq, _objects.begin(), _objects.end(), [staticCam](std::shared_ptr<Object> a, std::shared_ptr<Object> b)
              { return glm::distance(staticCam->getPosition(), a.get()->getPosition()) < glm::distance(staticCam->getPosition(), b.get()->getPosition()); });

    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    for (auto o : _objects)
        o.get()->draw(earlyZShader);
        
    glDepthFunc(GL_EQUAL);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_FALSE);
}