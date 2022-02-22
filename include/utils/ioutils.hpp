#ifndef IOUTILS_H
#define IOUTILS_H

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "engine.hpp"
#include "camera.hpp"

#define MOUSE_SENSITIVTY 0.07f

class IOUtils
{

private:
    // last mouse pointer position
    inline static double lastMouseX;
    inline static double lastMouseY;
    inline static bool resetFocus = false;
    inline static int lastDrawMode = 0;
    inline static Engine *engine;

public:
    static inline void setEngine(Engine &_engine) { engine = &_engine; }

    static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

    static void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);

    static void framebufferSizeCallback(GLFWwindow *window, int width, int height);

    static void mouseCallback(GLFWwindow *window, double xpos, double ypos);

    static void updateScreenRes(GLFWwindow *window, int width, int height);

    static void errorCallback(int error, const char *description)
    {
        fputs(description, stderr);
    }

private:
    static void onKeyPressed(GLFWwindow *window, int key);

    static void onKeyReleased(GLFWwindow *window, int key);
    
    static void onKeyRepeated(GLFWwindow *window, int key);
};

#endif