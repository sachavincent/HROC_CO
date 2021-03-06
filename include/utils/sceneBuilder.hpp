#ifndef SCENEGENERATOR_HPP
#define SCENEGENERATOR_HPP

#include "object.hpp"
#include "scene.hpp"
#include "meshhandler.hpp"
#include <memory>
#include <random>
#include <utility>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

//! Builds a scene for
class SceneBuilder
{

public:
    //! Builds the stating screen scene
    static Scene *buildDefaultScene(Engine *_engine)
    {
        Scene *sc = new Scene(_engine);
        auto light0 = std::make_shared<Light>(glm::vec3{-0.2, 0.25, -0.8}, glm::vec3{0.8f});
        sc->addLight(light0);

        auto sunLight = std::make_shared<Light>(glm::vec3{0, 50, -20}, glm::vec3{0.8f});
        sunLight->setAttenuation({1.0f, 0.0f, 0.0f});
        sc->addLight(sunLight);

        //####################### creating models ###########################

        MeshLoader::getSingleton()->generateBaseModels();
        MeshLoader::getSingleton()->loadModel("models/teapot.obj", "teapot");

        //####################### creating objects ###########################

        std::random_device device;
        std::mt19937 gen(device());
        std::uniform_real_distribution<float> dist(5, 20);
        std::uniform_real_distribution<float> distScale(1, 2);
        std::uniform_real_distribution<float> distRot(-1, 1);

        Texture::createTextureArray(1, 1024, 1024);
        for (int i = 0; i < 10; i++)
        {
            auto cube = std::make_shared<Cube>();
            cube->setPosition({dist(gen), dist(gen), dist(gen)})
                .setScale({distScale(gen), distScale(gen), distScale(gen)})
                .setRotation(distRot(gen) * 45, glm::vec3{distRot(gen), distRot(gen), distRot(gen)})
                .setDiffuse({0.0f, 1.0f, 0.3f})
                .setSpecular(glm::vec3{0.8});
            sc->addObject(cube);
        }

        auto plane1 = std::make_shared<Plane>();
        plane1->setPosition({10.0f, -3.0f, 0.0f})
            .setScale({20, 1, 20})
            .setTexDiffuse("textures/stoneWall/diffuse.png");
        sc->addObject(plane1);

        std::string keyModel("teapot");
        auto teapot = std::make_shared<FileObject>("models/teapot.obj", keyModel);
        teapot->setScale(glm::vec3{0.6f})
            .setPosition({10.0f, -1.5f, 0.0f})
            .setDiffuse({0.55f, 0.5f, 0.0f});
        sc->addObject(teapot);

        auto teapot2 = std::make_shared<FileObject>("models/teapot.obj", keyModel);
        teapot2->setScale(glm::vec3{0.7f})
            .setPosition({7.0f, -1.5f, -2.0f})
            .setDiffuse({0.12f, 0.1f, 0.7f});
        sc->addObject(teapot2);

        auto sphere1 = std::make_shared<UVSphere>();
        sphere1->setPosition({16.5, -1.8f, 3.5})
            .setRotation(90, {1, 0, 0})
            .setDiffuse({1.0, 0.0, 0.8f});

        sc->addObject(sphere1);
        auto sphere2 = std::make_shared<UVSphere>();
        sphere2->setPosition({7.5, -1.8f, 5.5})
            .setRotation(90, {1, 0, 0})
            .setDiffuse({1.0, 0.0, 0.0});

        sc->addObject(sphere2);
        return sc;
    }

    /** ! Builds an asteroid field scenewith given parameters
     * \param _size Size of the field in each direction
     * \param _center Center of the asteroid field
     * \param _aCount Number of asteroids in the field
     * \param _aSize Base size of an asteroid
     * \param _aScaleMult Random scaling factor [0,1]
     * */
    static Scene *buildAsteroidField(Engine *_engine, glm::vec3 _size = glm::vec3{150},
                                     glm::vec3 _center = {0, 0, 0}, uint32_t _aCount = 400,
                                     float _aSize = 1.0f, float _aScaleMult = 0.5f)
    {
        Scene *scene = new Scene(_engine);

        auto sunLight = std::make_shared<Light>(glm::vec3{0, 50, -20}, glm::vec3{0.8});
        sunLight->setAttenuation({1.0f, 0.0f, 0.0f});
        scene->addLight(sunLight);

        std::random_device device;
        std::mt19937 gen(device());
        std::uniform_real_distribution<float> distPosX(_center.x - _size.x / 2,
                                                       _center.x + _size.x / 2);
        std::uniform_real_distribution<float> distPosY(_center.y - _size.y / 2,
                                                       _center.y + _size.y / 2);
        std::uniform_real_distribution<float> distPosZ(_center.z - _size.z / 2,
                                                       _center.z + _size.z / 2);
        if (_aScaleMult <= 0.0f)
        {
            _aScaleMult = 0.0f;
        }
        else if (_aScaleMult >= 1.0f)
        {
            _aScaleMult = 0.999f;
        }
        std::uniform_real_distribution<float> distScale(_aSize - (_aSize * _aScaleMult),
                                                        _aSize + (_aSize * _aScaleMult));

        std::uniform_int_distribution<int> distRot(-1, 1);
        std::uniform_int_distribution<int> distAxisRot(0, 2);
        std::string modelKey = std::string("asteroid1");

        MeshLoader::getSingleton()->loadModel("models/asteroid/Asteroid1.obj", modelKey);

        Texture::createTextureArray(1, 1500, 1500);
        for (int i = 0; i < _aCount; i++)
        {
            auto asteroid = std::make_shared<FileObject>("models/asteroid/Asteroid1.obj", modelKey);
            int rotvalue = distAxisRot(gen);
            asteroid->setPosition({distPosX(gen), distPosY(gen), distPosZ(gen)})
                .setScale({distScale(gen), distScale(gen), distScale(gen)})
                .setRotation(distRot(gen) * 90.0f, {rotvalue == 0 ? 1.0f : 0.0f, rotvalue == 1 ? 1.0f : 0.0f, rotvalue == 2 ? 1.0f : 0.0f})
                .setTexDiffuse("models/asteroid/asteroidDiffuse.jpg");
            scene->addObject(asteroid);
        }
        return scene;
    }
    /**
     * @brief Build a scene from a given multi mesh .obj file
     *
     * @param _engine A pointer to current engine
     * @param _path The ABSOLUTE path of the .obj file
     * @return Scene* A pointer to the created scene
     */
    static Scene *buildMultiMesh(Engine *_engine, std::string _path, bool _absolutePath = false)
    {
        std::string path;
        if (!_absolutePath)
        {
            path = Utils::workingDirectory() + _path;
        }
        else
        {
            path = _path;
        }

        Scene *scene = new Scene(_engine);

        auto sunLight = std::make_shared<Light>(glm::vec3{0, 5000, -2000}, glm::vec3{0.4});
        sunLight->setAttenuation({1.0f, 0.0f, 0.0f});
        scene->addLight(sunLight);

        auto sunLight2 = std::make_shared<Light>(glm::vec3{0, 5000, 0}, glm::vec3{0.25});
        sunLight2->setAttenuation({1.0f, 0.0f, 0.0f});
        scene->addLight(sunLight2);

        static int mmeshinstance = 0;
        unsigned int numMeshesLoaded = 0;
        std::string meshKey = "mmesh" + std::to_string(mmeshinstance);
        MeshLoader::getSingleton()->loadModel(path, meshKey, &numMeshesLoaded);

        for (size_t i = 0; i < numMeshesLoaded - 1; i++)
        {
            auto mmesh = std::make_shared<FileObject>(path, meshKey + "_" + std::to_string(i));
            mmesh->setPosition(glm::vec3{0.0});
            scene->addObject(mmesh);
        }

        mmeshinstance++;
        return scene;
    }
};

#endif