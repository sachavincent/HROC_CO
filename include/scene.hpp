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
#include "boundingbox.hpp"
#include "bvhtree.hpp"

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

private:
    void load();

    void createBoundingBoxes();

    // Called when camera moves
    void updateBvh()
    {
        // TODO: Early-Z with V (at first = everything) => returns effectiveOccluders
        std::vector<BvhNode *> effectiveOccluders;
        std::vector<BvhNode *> occludeeGroups = hierarchy->extractOccludees(effectiveOccluders);
        // TODO: VFC => returns occludeeGroups filtered
        std::vector<BoundingBox *> occludeeGroups_boundingBoxes; // TODO: BvhNode => BoundingBox
        std::vector<BoundingBox *> potentiallyVisibleOccludees = batchOcclusionTest(occludeeGroups_boundingBoxes);
        // TODO: D <- effectiveOccluders
        // TODO: D <- D + success of Early-Z with potentiallyVisibleOccludees (object geometry not Bounding box)
        // TODO: Draw D
        // TODO: V <- These objects
    }

    std::vector<BoundingBox *> batchOcclusionTest(const std::vector<BoundingBox *> &occludeeGroups)
    {
        simpleShader.start();
        simpleShader.loadMat4("view", getCamera()->getViewMatrix());
        simpleShader.loadMat4("projection", getCamera()->getProjectionMatrix());

        unsigned int THRESHOLD = 10; // Min samples

        std::vector<BoundingBox *> potentiallyVisibleOccludees;
        // TODO: Sort boundingBoxes by distance
        const size_t nbQueries = occludeeGroups.size();
        GLuint *queries = new GLuint[nbQueries];
        glGenQueries(nbQueries, queries);

        unsigned int i = 0;
        for (BoundingBox *bb : occludeeGroups)
        {
            glBeginQuery(GL_SAMPLES_PASSED, queries[i++]);
            bb->getWireframe()->drawQuery(this);
            glEndQuery(GL_SAMPLES_PASSED);
        }

        for (unsigned int j = 0; j < i; j++)
        {
            unsigned int nbSamples;
            GLint available = GL_FALSE;
            while (available == GL_FALSE)
                glGetQueryObjectiv(queries[j], GL_QUERY_RESULT_AVAILABLE, &available);

            glGetQueryObjectuiv(queries[j], GL_QUERY_RESULT, &nbSamples);
            if (nbSamples > THRESHOLD)
                potentiallyVisibleOccludees.push_back(occludeeGroups[j]);
        }

        glDeleteQueries(nbQueries, queries);

        delete queries;

        simpleShader.stop();

        return potentiallyVisibleOccludees;
    }
};

#endif