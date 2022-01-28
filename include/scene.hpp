#ifndef SCENE_H
#define SCENE_H

#include <stdexcept>
#include <vector>
#include <iostream>
#include <memory>

#include "model.hpp"
#include "camera.hpp"
#include "light.hpp"
#include "boundingbox.hpp"

class Engine;

class Scene
{
public:
    Scene(Engine *_engine);

    Scene(Engine *_engine, const std::string &_file);

    void load();

    //! render models with standard shader (i.e. Phong)
    void renderModels();

    //! render a wireframe view of all bounding boxes in the scene.
    void renderBoundingBoxes();

    Scene &addModel(std::shared_ptr<Model> _model);

    Scene &addLight(std::shared_ptr<Light> _light);

    Scene &setExposure(float _exposure)
    {
        exposure = _exposure;
        return *this;
    }

    Camera *getCamera();

    inline float getExposure() const { return exposure; }

    inline const std::vector<std::shared_ptr<Light>> &getLights() { return lights; }

    inline const std::vector<std::shared_ptr<Model>> &getModels() { return models; }

private:
    std::vector<std::shared_ptr<Model>> models;
    std::vector<std::shared_ptr<Light>> lights;

    Engine *engine;

    float exposure;
};

#endif