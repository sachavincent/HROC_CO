#include "scene.hpp"
#include "engine.hpp"

Scene::Scene(Engine *_engine) : engine(_engine), exposure(1.0)
{

    auto light0 = std::make_shared<Light>(glm::vec3{-0.2, 0.25, -0.8}, glm::vec3{0.8f});
    addLight(light0);

    //####################### creating models ###########################

    auto cube0 = std::make_shared<Cube>(1.0f);
    cube0->setPosition({1.1f, 0.6f, 0.0f}).setDiffuse({0.0f, 1.0f, 0.0f}).setSpecular(glm::vec3{0.9});
    addModel(cube0);

    auto cube1 = std::make_shared<Cube>(1.0f);
    cube1->setPosition({-2.5f, 0.6f, 0.5f}).setScale({2.0, 1.0, 2.0}).setTexDiffuse("textures/tiles/basecolor.jpg").setTexSpecular("textures/tiles/roughness.png");
    addModel(cube1);

    auto sphere1 = std::make_shared<UVSphere>(1.0, 25, 20);
    sphere1->setPosition({3.5, 0.7, 3.5}).setRotation(-90, {1, 0, 0}).setDiffuse({1.0, 0.0, 1.0});

    addModel(sphere1);

    auto plane1 = std::make_shared<Plane>(glm::vec2{20, 20}, 30, 30);
    plane1->setRotation(-90, {1, 0, 0}).setTexDiffuse("textures/stoneWall/diffuse.png").setTexSpecular("textures/stoneWall/roughness.png").setTexScaling({4, 4});
    addModel(plane1);

    // gold-ish utah teapot
    auto teapot = std::make_shared<FileModel>("models/teapot.obj", SMOOTH_NORMAL_ENABLE);
    teapot->setScale(glm::vec3{0.45f}).setPosition({0.0f, 1.2f, -1.8f}).setDiffuse({0.55f, 0.5f, 0.0f});
    addModel(teapot);

    load();
    // TODO: Default scene
}

Scene::Scene(Engine *_engine, const std::string &_file) : engine(_engine), exposure(1.0)
{
    // TODO:
}

//! Load the scene models on GPU before rendering
void Scene::load()
{
    // load models
    for (uint32_t i = 0; i < models.size(); i++)
    {
        models[i]->load();
    }
}

//! Render all objects of scene
void Scene::renderModels()
{
    for (uint32_t i = 0; i < models.size(); i++)
    {
        models[i]->render(this);
    }
}

void Scene::renderBoundingBoxes()
{
    // TODO: this is only a testing code for this method
}

//! Add an object to scene
Scene &Scene::addModel(std::shared_ptr<Model> _model)
{
    models.push_back(_model);
    return *this;
}

//! Add a light to the scene
Scene &Scene::addLight(std::shared_ptr<Light> _light)
{
    lights.push_back(_light);
    return *this;
}

Camera *Scene::getCamera()
{
    return engine->getCurrentCamera();
}