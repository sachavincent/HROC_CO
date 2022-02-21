#ifndef SCENE_H
#define SCENE_H

#include <stdexcept>
#include <vector>
#include <map>
#include <iostream>
#include <memory>
#include <list>

#include "object.hpp"
#include "meshhandler.hpp"
#include "camera.hpp"
#include "light.hpp"
#include "bvh/boundingbox.hpp"
#include "bvh/bvhtree.hpp"
#include <string>
#include <time.h>
#include <chrono>
#include <ctime>
class Engine;

class Scene
{
private:
    Engine *engine;

    std::vector<std::shared_ptr<Object>> objects;
    std::vector<std::shared_ptr<Light>> lights;

    Shader sh;
    Shader simpleShader;
    Shader earlyZShader;
    Shader bbShader;
    Shader frustumShader;

    GLuint vao;
    GLuint vbo, ebo, inst, cmd, idVBO, colorVBO;
    GLuint ssbo;
    size_t nbObjects;
    float exposure;

    std::vector<glm::mat4> models;

    BvhTree *hierarchy;
    std::map<int, std::vector<std::shared_ptr<BoundingBoxObject>>, std::greater<int>> boundingBoxes;

    DrawElementsCommand *cmds, *earlyZcmds;
    int *visibility;

public:
    // Timers for pipeline
    double timers[9];
    const char *timerLabels[9]{"EarlyZ", "Extract", "VFC", "Raycast", "Bb extract", "Batch occlusion Test", "Early Z on Rendering", "Draw objects", "Merge"};

public:
    Scene(Engine *_engine);

    Scene(Engine *_engine, const std::string &_file);

    ~Scene();
    //! render objects with standard shader (i.e. Phong)
    void renderObjects();
    //! render one object with standard shader (i.e. Phong)
    void renderObject(Object &obj);
    //! Render all objects of given vector with standard shader (i.e. Phong)
    void renderObjects(std::vector<Object *> &vector);

    //! render a wireframe view of all bounding boxes in the scene.
    void renderBoundingBoxes();

    void renderFrustum();

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

    void load();

    void createBVH();

    void doEarlyZ(std::vector<std::shared_ptr<Object>> _objects);

private:
    void createFrustum();

    // Called when camera moves
    void updateBvh();

    std::vector<std::shared_ptr<BvhNode>> batchOcclusionTest(std::vector<std::shared_ptr<BvhNode>> &occludeeGroups);

    void setupEarlyZCommand()
    {
        std::vector<std::shared_ptr<Object>> _objects = objects;

        Camera *staticCam = getCamera();
        std::sort(/*std::execution::par_unseq, */ _objects.begin(), _objects.end(), [staticCam](std::shared_ptr<Object> a, std::shared_ptr<Object> b)
                  { return glm::distance(staticCam->getPosition(), a.get()->getPosition()) < glm::distance(staticCam->getPosition(), b.get()->getPosition()); });

        int cmdCount = 0;
        earlyZcmds = MeshHandler::getSingleton()->getCmds(_objects, &cmdCount);
        visibility = new int[_objects.size()];

        for (int i = 0; i < _objects.size(); ++i)
        {
            visibility[i] = 0;
        }

        glGenBuffers(1, &ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * _objects.size(), visibility, GL_DYNAMIC_COPY);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
    }
};

#endif