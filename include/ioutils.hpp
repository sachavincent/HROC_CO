#ifndef IOUTILS_H
#define IOUTILS_H

#include <iostream>

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "engine.hpp"
#include "camera.hpp"

#define MOUSE_SENSITIVTY 0.07f

class IOUtils
{

private:
    // last mouse pointer position
    inline static double _lastMouseX;
    inline static double _lastMouseY;
    inline static bool _resetFocus = false;
    inline static int _lastDrawMode = 0;
    inline static Camera *_camera;
    inline static Engine *_engine;

public:
    static void setEngine(Engine &engine);
    /**
     * Handles input and changes current camera to the given one
     */
    static void processInput(GLFWwindow *window, double deltaTime, Camera *camera);

    static void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);
    static void framebufferSizeCallback(GLFWwindow *window, int width, int height);
    static void mouseCallback(GLFWwindow *window, double xpos, double ypos);

    static void updateScreenRes(GLFWwindow *window, int width, int height);
};

#endif