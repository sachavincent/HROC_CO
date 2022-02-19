#ifndef JSONPARSER_H
#define JSONPARSER_H

class Scene;

#include <vector>
#include <iostream>
#include <memory>
#include <optional>
#include <functional>

#include <nlohmann/json.hpp>
using namespace nlohmann;

#include "object.hpp"

struct SceneData
{
    std::vector<std::shared_ptr<Object>> objects;
    std::vector<std::shared_ptr<Light>> lights;
    std::optional<std::function<void(Engine *)>> updateFreeCam;
    std::optional<std::function<void(Engine *)>> updateStaticCam;
};

class JsonParser
{
public:
    static glm::vec3 parseVec3(json jVec)
    {
        return glm::vec3(jVec["x"].get<float>(), jVec["y"].get<float>(), jVec["z"].get<float>());
    }

    static glm::vec2 parseVec2(json jVec)
    {
        return glm::vec2(jVec["x"].get<float>(), jVec["y"].get<float>());
    }

    static SceneData parseFile(std::ifstream &file)
    {
        SceneData sceneData;
        json j = json::parse(file);
        if (j.count("lights") != 0)
        {
            json jLights = j.at("lights");
            parseLights(jLights, sceneData);
        }
        if (j.count("cameras") != 0)
        {
            json jCameras = j.at("cameras");
            parseCameras(jCameras, sceneData);
        }
        if (j.count("objects") != 0)
        {
            json jObjects = j.at("objects");
            parseObjects(jObjects, sceneData);
        }
        return sceneData;
    }

    static void parseLights(json jLights, SceneData &sceneData)
    {
        std::vector<std::shared_ptr<Light>> lights;
        for (auto it = jLights.begin(); it != jLights.end(); it++)
        {
            auto light = std::make_shared<Light>();

            json jLight = *it;

            if (jLight.count("position") != 0)
                light->setPosition(parseVec3(jLight.at("position")));
            if (jLight.count("color") != 0)
                light->setColor(parseVec3(jLight.at("color")));
            if (jLight.count("attenuation") != 0)
                light->setAttenuation(parseVec3(jLight.at("attenuation")));
            lights.push_back(light);
        }

        sceneData.lights = lights;
    }

    static void parseCameras(json jCameras, SceneData &sceneData)
    {
        for (auto it = jCameras.begin(); it != jCameras.end(); it++)
        {
            json jCamera = *it;

            CameraInfo cameraInfo;
            if (jCamera.count("position") != 0)
                cameraInfo.position = parseVec3(jCamera.at("position"));
            if (jCamera.count("fov") != 0)
                cameraInfo.fov = jCamera.at("fov").get<float>();
            if (jCamera.count("yaw") != 0)
                cameraInfo.yaw = jCamera.at("yaw").get<float>();
            if (jCamera.count("pitch") != 0)
                cameraInfo.pitch = jCamera.at("pitch").get<float>();

            std::string type = jCamera.at("type").get<std::string>();
            if (type == "FREE")
            {
                sceneData.updateFreeCam = [cameraInfo](Engine *engine)
                {
                    engine->getFreeCam()->setCameraInfo(cameraInfo);
                };
            }
            else if (type == "STATIC")
            {
                sceneData.updateStaticCam = [cameraInfo](Engine *engine)
                {
                    engine->getStaticCamera()->setCameraInfo(cameraInfo);
                };
            }
            else
            {
                throw std::invalid_argument("Incorrect camera type='" + type + "'!");
            }
        }
    }

    static void parseObjects(json jObjects, SceneData &sceneData)
    {
        std::vector<std::shared_ptr<Object>> objects;
        for (auto it = jObjects.begin(); it != jObjects.end(); it++)
        {
            json jObject = *it;

            std::shared_ptr<Object> object;
            json jObjectMesh = jObject.at("mesh");
            std::string objType = jObjectMesh.at("type").get<std::string>();
            bool hasName = jObject.count("name") != 0;

            json args = jObjectMesh.at("args");
            if (objType == "Cube")
            {
                if (args.size() != 1)
                    throw std::invalid_argument("Incorrect arguments for object type='" + objType + "'!");

                float edgeSize = args[0].get<float>();
                if (hasName)
                    object = std::make_shared<Cube>(edgeSize, jObject.at("name").get<std::string>());
                else
                    object = std::make_shared<Cube>(edgeSize);
            }
            else if (objType == "UVSphere")
            {
                if (args.size() != 3)
                    throw std::invalid_argument("Incorrect arguments for object type='" + objType + "'!");

                float radius = args[0].get<float>();
                int nCols = args[1].get<int>();
                int nRows = args[2].get<int>();

                if (hasName)
                    object = std::make_shared<UVSphere>(radius, nCols, nRows, jObject.at("name").get<std::string>());
                else
                    object = std::make_shared<UVSphere>(radius, nCols, nRows);
            }
            else if (objType == "Plane")
            {
                if (args.size() != 3)
                    throw std::invalid_argument("Incorrect arguments for object type='" + objType + "'!");

                glm::vec2 size = parseVec2(args[0]);
                int nCols = args[1].get<int>();
                int nRows = args[2].get<int>();

                if (hasName)
                    object = std::make_shared<Plane>(size, nCols, nRows, jObject.at("name").get<std::string>());
                else
                    object = std::make_shared<Plane>(size, nCols, nRows);
            }
            else if (objType == "FileObject")
            {
                if (args.size() != 1)
                    throw std::invalid_argument("Incorrect arguments for object type='" + objType + "'!");

                object = std::make_shared<FileObject>(args[1].get<std::string>());
            }
            else
                throw std::invalid_argument("Unknown object type='" + objType + "'!");

            if (jObject.count("position") != 0)
                object->setPosition(parseVec3(jObject.at("position")));
            if (jObject.count("rotation") != 0 && jObject.count("rotation_axis") != 0)
                object->setRotation(jObject.at("rotation").get<float>(), parseVec3(jObject.at("rotation_axis")));
            if (jObject.count("scale") != 0)
                object->setScale(parseVec3(jObject.at("scale")));

            json jDiffuse = jObject.at("diffuse");
            std::string diffType = jDiffuse.at("type").get<std::string>();
            if (diffType == "color")
                object->setDiffuse(parseVec3(jDiffuse.at("value")));
            else if (diffType == "texture")
                object->setTexDiffuse(jDiffuse.at("value").get<std::string>());
            else
                throw std::invalid_argument("Incorrect object diffuse type='" + diffType + "'!");

            if (jObject.count("specular") != 0)
            {
                json jSpecular = jObject.at("specular");
                std::string specType = jSpecular.at("type").get<std::string>();
                if (specType == "color")
                    object->setSpecular(parseVec3(jSpecular.at("value")));
                else if (specType == "texture")
                    object->setTexSpecular(jSpecular.at("value").get<std::string>());
                else
                    throw std::invalid_argument("Incorrect object diffuse type='" + specType + "'!");
            }
            objects.push_back(object);
        }

        sceneData.objects = objects;
    }
};
#endif