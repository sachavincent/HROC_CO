#ifndef SCENE_H
#define SCENE_H

#include <stdexcept>
#include <vector>
#include <map>
#include <iostream>
#include <memory>

#include "object.hpp"
#include "camera.hpp"
#include "light.hpp"
#include "bvh/boundingbox.hpp"
#include "bvh/bvhtree.hpp"

class Engine;

class Scene
{
private:
    std::vector<std::shared_ptr<Object>> objects;
    std::vector<std::shared_ptr<Light>> lights;
    Shader sh;
    Shader simpleShader;
    Engine *engine;

    float exposure;
    //! maximum level of current bvh

    BvhTree *hierarchy;
    std::map<int, std::vector<std::shared_ptr<BoundingBoxObject>>, std::greater<int>> boundingBoxes;

public:
    Scene(Engine *_engine);

    Scene(Engine *_engine, const std::string &_file);

    ~Scene();
    //! render objects with standard shader (i.e. Phong)
    void renderObjects();
    //! render a wireframe view of all bounding boxes in the scene.
    void renderBoundingBoxes();

    Scene &addObject(std::shared_ptr<Object> _object);
    Scene &addLight(std::shared_ptr<Light> _light);

    Scene &setExposure(float _exposure)
    {
        exposure = _exposure;
        return *this;
    }

    Camera *getCamera();

    inline float getExposure() const { return exposure; }

    inline std::vector<std::shared_ptr<Light>> &getLights() { return lights; }

    inline std::vector<std::shared_ptr<Object>> &getObjects() { return objects; }

    Shader &getShader() { return sh; }

    Shader &getSimpleShader() { return simpleShader; }

    void load();

    void createBVH();

private:
    // Called when camera moves
    void updateBvh();
    

    std::vector<BoundingBox *> batchOcclusionTest(std::vector<BoundingBox *> &occludeeGroups);
};

#endif