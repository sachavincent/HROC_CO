#include "scene.hpp"
#include "engine.hpp"
#include "boundingBoxObject.hpp"
#include "utils/jsonparser.hpp"

#include <random>
#include <string>
#include <map>

Scene::Scene(Engine *_engine) : engine(_engine), exposure(1.0), hierarchy(nullptr)
{
    auto light0 =
        std::make_shared<Light>(glm::vec3{-0.2, 0.25, -0.8}, glm::vec3{0.8f});
    addLight(light0);

    auto sunLight =
        std::make_shared<Light>(glm::vec3{0, 50, -20}, glm::vec3{0.8f});
    sunLight->setAttenuation({1.0f, 0.0f, 0.0f});
    addLight(sunLight);

    //####################### creating models ###########################

    std::random_device device;
    std::mt19937 gen(device());
    std::uniform_int_distribution<int> dist(0, 50);

    // enough boxes to get down to 60fps in release
    for (size_t i = 0; i < 200; i++)
    {
        auto cube = std::make_shared<Cube>(1.0f);
        cube->setPosition({dist(gen), dist(gen), dist(gen)})
            .setDiffuse({0.0f, 1.0f, 0.0f})
            .setSpecular(glm::vec3{0.8});
        addObject(cube);
    }

    auto cube0 = std::make_shared<Cube>(1.0f);
    cube0->setPosition({1.1f, 0.6f, 0.0f})
        .setDiffuse({0.0f, 1.0f, 0.0f})
        .setSpecular(glm::vec3{0.9});
    // addObject(cube0);

    auto cube1 = std::make_shared<Cube>(1.0f);
    cube1->setPosition({-2.5f, 0.6f, 0.5f})
        .setScale({2.0, 1.0, 2.0})
        .setTexDiffuse("textures/tiles/basecolor.jpg")
        .setTexSpecular("textures/tiles/roughness.png");
    addObject(cube1);

    auto sphere1 = std::make_shared<UVSphere>(1.0, 25, 20);
    sphere1->setPosition({3.5, 0.7, 3.5})
        .setRotation(-90, {1, 0, 0})
        .setScale(glm::vec3{1.2f})
        .setDiffuse({1.0, 0.0, 1.0});

    addObject(sphere1);

    auto plane1 = std::make_shared<Plane>(glm::vec2{20, 20}, 30, 30);
    plane1->setRotation(-90, {1, 0, 0})
        .setTexDiffuse("textures/stoneWall/diffuse.png")
        .setTexSpecular("textures/stoneWall/roughness.png")
        .setTexScaling({4, 4});
    addObject(plane1);

    // gold-ish utah teapot
    auto teapot =
        std::make_shared<FileObject>("models/teapot.obj", true);
    teapot->setScale(glm::vec3{0.3f})
        .setPosition({0.0f, 1.2f, -1.8f})
        .setDiffuse({0.55f, 0.5f, 0.0f});
    // addObject(teapot);

    // TODO: Default scene
    load();

    createBoundingBoxes();
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
        sceneData.updateFreeCam(_engine);
        sceneData.updateStaticCam(_engine);
        lights = sceneData.lights;
        objects = sceneData.objects;

        load();

        createBoundingBoxes();
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
    delete hierarchy;
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
}

void Scene::createBoundingBoxes()
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
            objects[i]->draw(this);
        }
    }

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
                bbox.get()->draw(this, numBB++);
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
