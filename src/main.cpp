#include <iostream>

#include "engine.hpp"
#include "camera.hpp"
#include "scene.hpp"
#include "light.hpp"
#include "texture.hpp"

#include <glm/glm.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define WIDTH 1920
#define HEIGHT 1080

int main(int argc, char *argv[])
{

    // init the ui/camera/scene
    Ui ui {};

    Engine engine(WIDTH, HEIGHT);

    // Load scene
    engine.loadScene();

    // start the render loop
    engine.startLoop();

    return 0;
}
