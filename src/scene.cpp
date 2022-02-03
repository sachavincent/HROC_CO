#include "scene.hpp"
#include "engine.hpp"

#include <random>


Scene::Scene(Engine *_engine) : engine(_engine), exposure(1.0) {
    auto light0 =
        std::make_shared<Light>(glm::vec3{-0.2, 0.25, -0.8}, glm::vec3{0.8f});
    addLight(light0);


    auto sunLight =
        std::make_shared<Light>(glm::vec3{0, 50, -20}, glm::vec3{0.8f});
    sunLight->setAttenuation({1.0f,0.0f,0.0f});
    addLight(sunLight);

    //####################### creating models ###########################

    std::random_device device;
    std::mt19937 generator(device());
    std::uniform_int_distribution<int> distribution(0,100);

    // enough boxes to get down to 60fps in release
    for(size_t i = 0; i<1000;i++){
        auto cube = std::make_shared<Cube>(1.0f);
        cube->setPosition({distribution(generator)-50, distribution(generator), distribution(generator)+10})
        .setDiffuse({0.0f, 1.0f, 0.0f})
        .setSpecular(glm::vec3{0.8});
        addObject(cube);
    }

    auto cube0 = std::make_shared<Cube>(1.0f);
    cube0->setPosition({1.1f, 0.6f, 0.0f})
        .setDiffuse({0.0f, 1.0f, 0.0f})
        .setSpecular(glm::vec3{0.9});
    addObject(cube0);

    auto cube1 = std::make_shared<Cube>(1.0f);
    cube1->setPosition({-2.5f, 0.6f, 0.5f})
        .setScale({2.0, 1.0, 2.0})
        .setTexDiffuse("textures/tiles/basecolor.jpg")
        .setTexSpecular("textures/tiles/roughness.png");
    addObject(cube1);

    auto sphere1 = std::make_shared<UVSphere>(1.0, 25, 20);
    sphere1->setPosition({3.5, 0.7, 3.5})
        .setRotation(-90, {1, 0, 0})
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
        std::make_shared<FileObject>("models/teapot.obj", SMOOTH_NORMAL_ENABLE);
    teapot->setScale(glm::vec3{0.3f})
        .setPosition({0.0f, 1.2f, -1.8f})
        .setDiffuse({0.55f, 0.5f, 0.0f});
    addObject(teapot);

    load();
    // TODO: Default scene

    std::vector<BoundingBox *> bbs;
    for (auto obj : getObjects())
    {
        Object *bbo = obj.get();
        if (bbo && bbo->getBoundingBox())
            bbs.push_back(bbo->getBoundingBox());
    }

    hierarchy = new BvhTree(bbs);
}

Scene::Scene(Engine *_engine, const std::string &_file)
    : engine(_engine), exposure(1.0)
{
    // TODO:
}

//! Load the scene models on GPU before rendering
void Scene::load()
{
    // load models
    for (size_t i = 0; i < objects.size(); i++)
    {
        objects[i]->load();
    }
    //load shader
    sh = {"shaders/default.vert", "shaders/phong.frag"};
}

//! Render all objects of scene
void Scene::renderObjects() {
    // set shaders params
    sh.start();

    sh.loadMat4("view", getCamera()->getViewMatrix());
    sh.loadMat4("projection", getCamera()->getProjectionMatrix());
    sh.loadFloat("exposure", exposure);
    sh.loadVec3("viewPos", getCamera()->getPosition());

    for (uint32_t i = 0; i < std::min(lights.size(), (size_t)MAXLIGHTS); i++) {
        sh.loadBool("lights[" + std::to_string(i) + "].enabled", 1);

        sh.loadVec3("lights[" + std::to_string(i) + "].position",
                        lights[i]->getPosition());

        sh.loadVec3("lights[" + std::to_string(i) + "].color",
                        lights[i]->getColor());
        sh.loadVec3("lights[" + std::to_string(i) + "].attenuation",
                        lights[i]->getAttenuation());
    }
    if (lights.size() < MAXLIGHTS) {
        for (size_t i = lights.size(); i < MAXLIGHTS; i++) {
            sh.loadBool("lights[" + std::to_string(i) + "].enabled", 0);
        }
    }

    //draw objects
    for (size_t i = 0; i < objects.size(); i++) {
        objects[i]->draw(this);
    }

    //unload shader
    sh.stop();
}

void Scene::renderBoundingBoxes()
{
    // TODO: this is only a testing code for this method
    if (!hierarchy)
        return;

    auto debugData = hierarchy->getDebugData();
    for (auto entry : debugData)
    {
        auto bbs = entry.second;
        for (auto bb : bbs)
        {
            bb->draw(this, entry.first);
        }
    }
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
