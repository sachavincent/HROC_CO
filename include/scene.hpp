#ifndef SCENE_H
#define SCENE_H

#include <stdexcept>
#include <vector>
#include <map>
#include <iostream>
#include <memory>
#include <list>

#include "object.hpp"
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

    GLuint vao;
    GLuint vbo, ebo, inst, cmd, idVBO, colorVBO;
    GLuint ssbo;
    size_t nbObjects;
    float exposure;

    std::vector<glm::mat4> models;

    BvhTree *hierarchy;
    std::map<int, std::vector<std::shared_ptr<BoundingBoxObject>>, std::greater<int>> boundingBoxes;
    DrawElementsCommand *earlyZcmds;
    DrawElementsCommand *cmds;
    int *visibility;
    //std::set<OBJECT_DATA>
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

    // Timers for pipeline
    double timers[9];
    const char *timerLabels[9]{"EarlyZ", "Extract", "VFC", "Raycast", "Bb extract", "Batch occlusion Test", "Early Z on Rendering", "Draw objects", "Merge"};

    void doEarlyZ(std::vector<std::shared_ptr<Object>> _objects);

private:
    // Called when camera moves
    void updateBvh();

    std::vector<BoundingBox *> batchOcclusionTest(std::vector<BoundingBox *> &occludeeGroups);

    void setupEarlyZCommand()
    {
        std::vector<std::shared_ptr<Object>> _objects = objects;

        Camera *staticCam = getCamera();
        std::sort(/*std::execution::par_unseq, */ _objects.begin(), _objects.end(), [staticCam](std::shared_ptr<Object> a, std::shared_ptr<Object> b)
                  { return glm::distance(staticCam->getPosition(), a.get()->getPosition()) < glm::distance(staticCam->getPosition(), b.get()->getPosition()); });

        // TEAPOT x1
        // Plane x1
        // TEAPOT x1
        // SPHERE x2
        // CUBE x10
        earlyZcmds = new DrawElementsCommand[5];

        std::vector<OBJECT_DATA> objD = FileObject::getData();
        OBJECT_DATA teapotData = objD[0];
        OBJECT_DATA cubeData = Cube::getData();
        OBJECT_DATA planeData = Plane::getData();
        OBJECT_DATA sphereData = UVSphere::getData();

        GLuint baseVert = 0;
        GLuint baseIdx = 0;
        GLuint baseInstance = 0;
        earlyZcmds[0].vertexCount = teapotData.numIndices;
        earlyZcmds[0].instanceCount = 1;
        earlyZcmds[0].firstIndex = 42;
        earlyZcmds[0].baseVertex = 40;
        earlyZcmds[0].baseInstance = 11;
        baseIdx += teapotData.numIndices;
        baseInstance += 1;

        earlyZcmds[1].vertexCount = planeData.numIndices;
        earlyZcmds[1].instanceCount = 1;
        earlyZcmds[1].firstIndex = 36;
        earlyZcmds[1].baseVertex = 36;
        earlyZcmds[1].baseInstance = 10;
        baseInstance += 1;

        earlyZcmds[2].vertexCount = teapotData.numIndices;
        earlyZcmds[2].instanceCount = 1;
        earlyZcmds[2].firstIndex = 42;
        earlyZcmds[2].baseVertex = 40;
        earlyZcmds[2].baseInstance = 12;
        baseInstance += 1;

        earlyZcmds[3].vertexCount = sphereData.numIndices;
        earlyZcmds[3].instanceCount = 2;
        earlyZcmds[3].firstIndex = 19002;
        earlyZcmds[3].baseVertex = 19000;
        earlyZcmds[3].baseInstance = 13;
        baseInstance += 2;

        earlyZcmds[4].vertexCount = cubeData.numIndices;
        earlyZcmds[4].instanceCount = 10;
        earlyZcmds[4].firstIndex = 0;
        earlyZcmds[4].baseVertex = 0;
        earlyZcmds[4].baseInstance = 0;

        visibility = new int[objects.size()];

        for (size_t i = 0; i < objects.size(); i++)
            visibility[i] = 0;
        glGenBuffers(1, &ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * objects.size(), visibility, GL_DYNAMIC_COPY);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
    }
};

#endif