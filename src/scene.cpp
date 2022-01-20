#include "scene.hpp"

Scene::Scene(Camera& _cam) : cam(_cam) {
    models = {};
    lights = {};
}


//! Load the scene models on GPU before rendering
void Scene::load() {
    //load cubemap
    if (cubeMap != nullptr) cubeMap->load();
    //load models
    for (uint32_t i = 0; i < models.size(); i++) {
        models[i]->load();
    }
}

//! render cubeMap, this is the first object to render
void Scene::renderCubeMap() {
    if (cubeMap != nullptr) cubeMap->render(this);
}

//! Render all objects of scene
void Scene::renderModels() {
    for (uint32_t i = 0; i < models.size(); i++) {
        models[i]->render(this);
    }
}

void Scene::renderModelsWithShader(Shader& _shader) {
    for (uint32_t i = 0; i < models.size(); i++) {
        models[i]->renderForDepth(_shader);
    }
}

//! Add and object to scene
Scene& Scene::addModel(Model& _model) {
    models.push_back(&_model);
    return *this;
}

//! Add a light to the scene
Scene& Scene::addLight(Light& _light) {
    lights.push_back(&_light);
    return *this;
}

Scene& Scene::setCubeMap(CubeMap& _cubeMap) {
    cubeMap = &_cubeMap;
    return *this;
}
Scene& Scene::setCamera(Camera& _cam){
    cam = _cam;
    return *this;
}

