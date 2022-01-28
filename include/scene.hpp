#ifndef SCENE_H
#define SCENE_H

#include <stdexcept>
#include <vector>
#include <iostream>
#include <memory>

#include "object.hpp"
#include "camera.hpp"
#include "light.hpp"
#include "boundingbox.hpp"

class Scene
{

private:
    std::vector<std::shared_ptr<Object>> objects;
    std::vector<std::shared_ptr<Light>> lights;

    Camera &cam;

    float exposure = 1.0;

public:
    Scene(Camera &_cam);
    Scene(Camera &_cam, const std::string &file);

    void load();
    //! render objects with standard shader (i.e. Phong)
    void renderObjects();
    //! render a wireframe view of all bounding boxes in the scene.
    void renderBoundingBoxes();

    Scene &addObject(std::shared_ptr<Object> _object);
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

    inline const std::vector<std::shared_ptr<Object>> &getObjects() { return objects; }
};

#endif