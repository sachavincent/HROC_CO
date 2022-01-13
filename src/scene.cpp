#include "scene.hpp"

Scene::Scene(Camera& _cam) : cam(_cam) {
    models = {};
    lights = {};
}


//! Load the scene models on GPU before rendering
void Scene::load() {
    // create depth buffers for shadow map enabled lights
    for (uint32_t i = 0; i < lights.size(); i++) {
        if (lights[i]->hasShadowMap()) {
            DistantLight* li = dynamic_cast<DistantLight*>(lights[i]);
            li->createDepthBuffer();
        }
    }
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

//! render depth map for each shadow map enabled light in scene
void Scene::depthMaps_pass() {
    for (uint32_t i = 0; i < lights.size(); i++) {
        if (lights[i]->hasShadowMap()) {
            DistantLight* li = dynamic_cast<DistantLight*>(lights[i]);
            Shader sh = {"shaders/depthmap/depthmap.vert", 
                        "shaders/depthmap/depthmap.frag"};
            
            sh.use();
            sh.setMat4("vp", li->getLightSpacematrix());

            uint32_t res = li->getSMRes();
            glViewport(0, 0, res, res);
            glBindFramebuffer(GL_FRAMEBUFFER, li->getFbo());
            glClear(GL_DEPTH_BUFFER_BIT);

            renderModelsWithShader(sh);
            
            // reset
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, cam.getResWidth(), cam.getResHeight());
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
    }
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

