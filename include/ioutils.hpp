#ifndef IOUTILS_H
#define IOUTILS_H

#include <iostream>


#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
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
    inline static Engine *_engine;

public:
    static inline void setEngine(Engine &engine) { _engine = &engine; }

    static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

    static void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);

    static void framebufferSizeCallback(GLFWwindow *window, int width, int height);

    static void mouseCallback(GLFWwindow *window, double xpos, double ypos);

    static void updateScreenRes(GLFWwindow *window, int width, int height);

    static void errorCallback(int error, const char *description) // TODO: Enlever quand le projet est fini
    {
        fputs(description, stderr);
    }

private:
    static void onKeyPressed(GLFWwindow *window, int key);
    
    static void onKeyReleased(GLFWwindow *window, int key);
};

#endif