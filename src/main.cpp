#include <iostream>

#include "camera.hpp"
#include "engine.hpp"
#include "light.hpp"
#include "scene.hpp"
#include "texture.hpp"
#include "utils/sceneBuilder.hpp"

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif

#include <glm/glm.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define WIDTH 1280
#define HEIGHT 720

int main(int argc, char *argv[])
{

    // init the ui/camera/scene
    Ui ui{};

    Engine engine(WIDTH, HEIGHT);

    // engine.loadScene("testScene.json");




    // base scene loaded in main
    //Scene* testScene = SceneBuilder::buildMultiMesh(&engine,"models/paul_sab/paul_sab_527.obj");
    Scene* testScene = SceneBuilder::buildDefaultScene(&engine);
    //Scene* testScene =  SceneBuilder::buildAsteroidField(&engine, glm::vec3{35,35,100}, glm::vec3{0.0f,0.0f,55.0f}, 300, 1.0, 0.1);
    engine.loadScene(testScene);
    testScene->createBVH();


    // TODO: following procedure should be automated in gui
    // delete previous -> build a scene -> load it to engine -> generate BVH -> repeat
    // delete testScene;
    // testScene =  SceneBuilder::buildAsteroidField(&engine, glm::vec3{40,40,150}, glm::vec3{0.0f,0.0f,70.0f}, 2000, 0.8, 0.5);
    // engine.loadScene(testScene);
    // testScene->createBVH();


    //  start the render loop
    engine.startLoop();

    return 0;
}
