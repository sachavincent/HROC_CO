#ifndef SCENE_H
#define SCENE_H

#include <stdexcept>

#include "model.hpp"
#include "headers.hpp"
#include "camera.hpp"
#include "light.hpp"
#include "ssao.hpp"


class Scene{
    
private:
    std::vector<Model*> models;
    std::vector<Light*> lights;

    Camera& cam; 

    SSAO* ssao = nullptr;
    bool ssaoEnabled = false;

    float exposure = 1.0;

    CubeMap* cubeMap = nullptr;

public:
    Scene(Camera& _cam);
    ~Scene();

    void load();
    void renderCubeMap();
    void SSAO_Pass();
    void depthMaps_pass();
    void renderModels();

    
    
    //! Render all models of scene using the same shader
    /**
     * \param _shader The shader to render the models with,
     * only "model" matrix is defined per object.
     **/
    void renderModelsWithShader(Shader& _shader);

    Scene& addModel(Model& _model);
    Scene& addLight(Light& _light);
    Scene& setCubeMap(CubeMap& _cubeMap);
    Scene& setCamera(Camera& _cam);

    Scene& setExposure(float _exposure){exposure = _exposure;return *this;}
    Scene& setSSAO(bool _status){ssaoEnabled = _status;return *this;};
    float getExposure(){return exposure;}
    Camera& getCam(){return cam;}
    SSAO* getSSAO(){return ssao;}
    bool SSAOstatus();
    std::vector<Light*>& getLights(){return lights;}
    std::vector<Model*>& getModels(){return models;}
};

#endif