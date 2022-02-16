#ifndef SCENEGENERATOR_HPP
#define SCENEGENERATOR_HPP

#include "scene.hpp"
#include "object.hpp"

#include <utility>
#include <memory>
#include <random>

//! Builds a scene for
class SceneBuilder
{

public:
    //! Builds the stating screen scene
    static Scene *buildDefaultScene(Engine *_engine)
    {
        Scene *sc = new Scene(_engine);
        auto light0 = std::make_shared<Light>(glm::vec3{-0.2, 0.25, -0.8},
                                              glm::vec3{0.8f});
        sc->addLight(light0);

        auto sunLight =
            std::make_shared<Light>(glm::vec3{0, 50, -20}, glm::vec3{0.8f});
        sunLight->setAttenuation({1.0f, 0.0f, 0.0f});
        sc->addLight(sunLight);

        //####################### creating models ###########################

        std::mt19937 gen(12);
        std::uniform_real_distribution<float> dist(-50, 50);
        std::uniform_real_distribution<float> distScale(1, 2);
        std::uniform_real_distribution<float> distRot(-1, 1);

        for (size_t i = 0; i < 10; i++)
        {
            auto cube = std::make_shared<Cube>(1.0f);
            cube->setPosition({dist(gen), dist(gen), dist(gen)})
                .setScale({distScale(gen), distScale(gen), distScale(gen)})
                .setRotation(distRot(gen) * 45, glm::vec3{distRot(gen), distRot(gen), distRot(gen)})
                .setDiffuse({0.0f, 1.0f, 0.0f})
                .setSpecular(glm::vec3{0.8});
            sc->addObject(cube);
        }
        return sc;
    }

    /** ! Builds an asteroid field scenewith given parameters
     * \param _size Size of the fields in each direction
     * \param _center Centroid of the asteroid field
     * \param _aCount Number of asteroids int the field
     * \param _aSize base size of an asteroid
     * \param _aScaleMult affect randomnessscaling of asteroids
     * */
    Scene *buildAsteroidField(glm::vec3 _size, glm::vec3 _center,
                              uint32_t _aCount, float _aSize, float _aScaleMult);

    /** ! Builds an simple city scene with given parameters
     * \param _citySize Size of the city (in tiles) for each dir.
     * \param _cityScale Overall city scale multiplier.
     * \param _bldBaseSize Base building height
     * \param _bldSizeMult Building height radnomness multiplier
     * \param _bldDensity Density of buildings in city [0,1.0]
     * \param _street2BldMult
     * \returns Path of the created scene
     * */
    std::string buildCity(std::pair<uint32_t, uint32_t> _citySize,
                          float _cityScale, float _bldBaseSize,
                          float _bldSizeMult, float _bldDensity,
                          float _street2BldMult);

    /** ! Builds an simple city scene with given parameters
     * \param _size Size of the city (in tiles) for each dir.
     * \param _cityScale Overall city scale multiplier.
     * \param _bldBaseSize Base building height
     * \param _bldSizeMult Building height radnomness multiplier
     * \param _bldDensity Density of buildings in city [0,1.0]
     * \param _street2BldMult
     * */
    Scene *buildForest(glm::vec3 size);
};

#endif