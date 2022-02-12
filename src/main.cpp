#include <iostream>

#include "engine.hpp"
#include "camera.hpp"
#include "scene.hpp"
#include "light.hpp"
#include "texture.hpp"

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif

#include <glm/glm.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define WIDTH 1920
#define HEIGHT 1080

int main(int argc, char *argv[])
{

    // init the ui/camera/scene
    Ui ui{};

    Engine engine(WIDTH, HEIGHT);

    // Load scene
    engine.loadScene("testScene.json");
    //engine.loadScene();
    //  start the render loop
    engine.startLoop();

    return 0;
}
