#ifndef INPUTS_H
#define INPUTS_H

#include "headers.hpp"
#include "camera.hpp"


class Inputs{

private:
    // last mouse pointer position
    inline static double lastMouseX;
    inline static double lastMouseY;
    inline static float mouseSensitivity;
    inline static Camera* cam;
    inline static bool resetFocus;
    inline static int lastDrawMode;

public:
    Inputs(){lastDrawMode=0;}
    Inputs(Camera* _cam);
    
    void setCamera(Camera* _cam){
        cam = _cam;
    };

    void processInput(GLFWwindow* window, double deltaTime);

    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);

};

#endif