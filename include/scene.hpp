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
#include "bvh/bvh.hpp"
#include "bvh/bounding_box.hpp"
#include "bvh/boundingBoxObject.hpp"
#include "bvh/utilities.hpp"

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

    bvh::Bvh *hierarchy;
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
    void updateBvh()
    {
        // TODO: Early-Z with V (at first = everything) => returns effectiveOccluders
        std::vector<unsigned int> effectiveOccluders;
        std::vector<unsigned int> occludeeGroups = hierarchy->extractOccludees(effectiveOccluders); // = G
        // TODO: VFC => returns occludeeGroups filtered
        std::vector<BoundingBox *> occludeeGroups_boundingBoxes;
        // TODO: BvhNode => BoundingBox
        std::vector<BoundingBox *> potentiallyVisibleOccludees = batchOcclusionTest(occludeeGroups_boundingBoxes); // = U
        // TODO: D <- effectiveOccluders
        // TODO: D <- D + success of Early-Z with potentiallyVisibleOccludees (object geometry not Bounding box)
        // TODO: Draw D
        // TODO: V <- These objects
    }

    std::vector<BoundingBox *> batchOcclusionTest(std::vector<BoundingBox *> &occludeeGroups);
};

#endif