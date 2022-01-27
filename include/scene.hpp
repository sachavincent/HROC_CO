#ifndef SCENE_H
#define SCENE_H

#include <stdexcept>
#include <vector>
#include <iostream>
#include <memory>

#include "model.hpp"
#include "camera.hpp"
#include "light.hpp"

class Scene
{

private:
    std::vector<std::shared_ptr<Model>> models;
    std::vector<std::shared_ptr<Light>> lights;

    Camera &cam;

    float exposure = 1.0;

public:
    Scene(Camera &_cam);
    Scene(Camera &_cam, const std::string &file);

    void load();
    void renderModels();

    //! Render all models of scene using the same shader
    /**
     * \param _shader The shader to render the models with,
     * only "model" matrix is defined per object.
     **/
    void renderModelsWithShader(Shader &_shader);

    Scene &addModel(std::shared_ptr<Model> _model);
    Scene &addLight(std::shared_ptr<Light> _light);
    Scene &setCamera(Camera &_cam);

    Scene &setExposure(float _exposure)
    {
        exposure = _exposure;
        return *this;
    }
    float getExposure() { return exposure; }
    Camera &getCamera() { return cam; }
    Camera &getFreeCam() { return cam; }

    inline const std::vector<std::shared_ptr<Light>> &getLights() { return lights; }

    inline const std::vector<std::shared_ptr<Model>> &getModels() { return models; }
};

#endif