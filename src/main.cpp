#include <iostream>

#include "mainLoop.hpp"
#include "camera.hpp"
#include "scene.hpp"
#include "light.hpp"
#include "texture.hpp"

#include <glm/glm.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

int main(int argc, char *argv[])
{

    int windowWidth = 1200;
    int windowHeight = 800;

    //init the interface/camera/scene
    Ui interface{};
    Camera cam{glm::vec3{0, 4, -8}, 50.0f, windowWidth, windowHeight};

    Scene scene{cam};
    scene.setExposure(1.0);

    MainLoop renderLoop{scene, interface, cam};
    // ######################### adding lights ###########################

    Light light3{glm::vec3{-0.2, 0.25, -0.8}, glm::vec3{0.8f}};
    scene.addLight(light3);

    //create a cubemap and add it to the scene
    CubeMap cubemap{"textures/cubemaps/yokohama/"};
    scene.setCubeMap(cubemap);

    //####################### creating models ###########################

    Cube cube0{1.0f};
    cube0.setPosition({1.1f, 0.6f, 0.0f}).setDiffuse({0.0f, 1.0f, 0.0f}).setSpecular(glm::vec3{0.9});

    Cube cube1(1.0f);
    cube1.setPosition({-2.5f, 0.6f, 0.5f}).setScale({2.0, 1.0, 2.0}).setTexDiffuse("textures/tiles/basecolor.jpg").setTexSpecular("textures/tiles/roughness.png");

    scene.addModel(cube0).addModel(cube1);

    UVSphere sphere1(1.0, 25, 20);
    sphere1.setPosition({3.5, 0.7, 3.5}).setRotation(-90, {1, 0, 0}).setDiffuse({1.0, 0.0, 1.0});

    scene.addModel(sphere1);

    Plane plane1{{20, 20}, 30, 30};
    plane1.setRotation(-90, {1, 0, 0}).setTexDiffuse("textures/stoneWall/diffuse.png").setTexSpecular("textures/stoneWall/roughness.png").setTexScaling({4, 4});
    scene.addModel(plane1);

    //gold-ish utah teapot
    FileModel teapot{"models/teapot.obj", SMOOTH_NORMAL_ENABLE};
    teapot.setScale(glm::vec3{0.45f}).setPosition({0.0f, 1.2f, -1.8f}).setDiffuse({0.55f, 0.5f, 0.0f});

    scene.addModel(teapot);

    // ########## start the render loop, give it the scene/cameras #########

    // first, load scene objects to GPU
    scene.load();
    //start the render loop
    renderLoop.run();

    return 0;
}
