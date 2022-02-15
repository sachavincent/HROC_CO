#ifndef SCENEGENERATOR_HPP
#define SCENEGENERATOR_HPP

#include "object.hpp"
#include "scene.hpp"

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

        std::random_device device;
        std::mt19937 gen(device());
        std::uniform_real_distribution<float> dist(5, 20);
        std::uniform_real_distribution<float> distScale(1, 2);
        std::uniform_real_distribution<float> distRot(-1, 1);

        for (size_t i = 0; i < 10; i++)
        {
            auto cube = std::make_shared<Cube>(1.0f);
            cube->setPosition({dist(gen), dist(gen), dist(gen)})
                .setScale({distScale(gen), distScale(gen), distScale(gen)})
                .setRotation(distRot(gen) * 45, glm::vec3{distRot(gen), distRot(gen), distRot(gen)})
                .setTexDiffuse("textures/stoneWall/diffuse.png")
                .setTexSpecular("textures/stoneWall/roughness.png");
            //.setDiffuse({0.0f, 1.0f, 0.0f})
            //.setSpecular(glm::vec3{0.8});
            sc->addObject(cube);
        }

        auto sphere1 = std::make_shared<UVSphere>(1.0, 25, 20);
        sphere1->setPosition({3.5, 0.7, 3.5})
            .setRotation(90, {1, 0, 0})
            .setDiffuse({1.0, 0.0, 1.0});

        sc->addObject(sphere1);

        auto plane1 = std::make_shared<Plane>(glm::vec2{20, 20}, 30, 30);
        plane1->setRotation(-90, {1, 0, 0})
            .setTexDiffuse("textures/stoneWall/diffuse.png")
            .setTexSpecular("textures/stoneWall/roughness.png")
            .setTexScaling({4, 4});
        sc->addObject(plane1);

        auto teapot = std::make_shared<FileObject>("models/teapot.obj", true);
        teapot->setScale(glm::vec3{0.6f})
            .setPosition({0.0f, 1.5f, 0.0f})
            .setDiffuse({0.55f, 0.5f, 0.0f});
        sc->addObject(teapot);
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

        std::uniform_int_distribution<int> distRot(-1,1);
        std::uniform_int_distribution<int> distAxisRot(0,2);
        for (size_t i = 0; i < _aCount; i++)
        {
            auto asteroid = std::make_shared<FileObject>("models/asteroid/Asteroid1.obj", false);
            asteroid->setPosition({distPosX(gen), distPosY(gen), distPosZ(gen)})
                .setScale(glm::vec3{distScale(gen),distScale(gen),distScale(gen)})
                .setRotation(distRot(gen)*90,{distAxisRot(gen)==0?1:0,distAxisRot(gen)==1?1:0,distAxisRot(gen)==2?1:0})
                .setTexDiffuse("models/asteroid/asteroidDiffuse.jpg")
                .setTexSpecular("models/asteroid/asteroidSpecular.png");
            scene->addObject(asteroid);
        }
        return scene;
    }

    static Scene *buildMultiMesh(Engine *_engine, std::string _path)
    {
        std::string abs_path = Utils::workingDirectory() + _path;
        
        Scene *scene = new Scene(_engine);

        auto sunLight = std::make_shared<Light>(glm::vec3{0, 5000, -2000}, glm::vec3{0.5});
        sunLight->setAttenuation({1.0f, 0.0f, 0.0f});
        scene->addLight(sunLight);
        auto sunLight2 = std::make_shared<Light>(glm::vec3{0, 5000, -2000}, glm::vec3{0.4});
        sunLight2->setAttenuation({1.0f, 0.0f, 0.0f});
        scene->addLight(sunLight2);
        auto sunLight3 = std::make_shared<Light>(glm::vec3{0, 5000, 0}, glm::vec3{0.25});
        sunLight3->setAttenuation({1.0f, 0.0f, 0.0f});
        scene->addLight(sunLight3);

        std::cout << "loading city from file : " << abs_path << " ..." << std::endl;

        Assimp::Importer importer;
        const aiScene *assimpScene = importer.ReadFile(abs_path, aiProcess_Triangulate | aiProcess_FlipUVs |
                                                             aiProcess_GenNormals);


        if (!scene || assimpScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !assimpScene->mRootNode)
        {
            std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return scene;
        }

        std::string name;
        for(size_t i = 0; i< assimpScene->mNumMeshes;i++){
            name = _path+std::to_string(i);
            auto mesh = std::make_shared<AssimpMeshObject>(assimpScene,assimpScene->mMeshes[i],name);
            scene->addObject(mesh);
        }
        return scene;
    }

    /** ! Builds an simple city scene with given parameters
     * \param _size Size of the city (in tiles) for each dir.
     * \param _cityScale Overall city scale multiplier.
     * \param _bldBaseSize Base building height
     * \param _bldSizeMult Building height radnomness multiplier
     * \param _bldDensity Density of buildings in city [0,1.0]
     * \param _street2BldMult
     * */
    static Scene *buildForest(glm::vec3 size);
};

#endif