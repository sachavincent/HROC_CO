#include "inputs.hpp"

Inputs::Inputs(Camera* _cam){
    cam = _cam;
    mouseSensitivity = 0.07f;
    lastMouseX = cam->getResWidth()/2; 
    lastMouseY = cam->getResHeight()/2;
    resetFocus = true;
}


void Inputs::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    resetFocus = true;
    cam->setRes(width,height);
}

void Inputs::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if(!resetFocus){
        double xoffset = xpos - lastMouseX;
        double yoffset = lastMouseY - ypos;

        lastMouseX = xpos;
        lastMouseY = ypos;
        cam->offsetYaw(xoffset, mouseSensitivity);
        cam->offsetPitch(yoffset, mouseSensitivity);
    } else {
        lastMouseX = cam->getResWidth()/2;
        lastMouseX = cam->getResHeight()/2;
        glfwSetCursorPos(window,lastMouseX, lastMouseY);
        resetFocus = false;
    }
    
}

void Inputs::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    cam->offsetFov(yoffset);
}

// manage inputs
void Inputs::processInput(GLFWwindow* window, double deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
        
    // keys 1,2,3 to switch polygon display mode
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

    // camera movement
    const float cameraSpeed = 2.0f * (float)deltaTime;  // framerate independent

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cam->moveZ(cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cam->moveZ(-cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cam->moveX(-cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cam->moveX(cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) cam->moveZ(cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) cam->moveZ(-cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) cam->moveX(-cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) cam->moveX(cameraSpeed);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cam->moveY(cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        cam->moveY(-cameraSpeed);

    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetCursorPosCallback(window, nullptr);
        glfwSetScrollCallback(window, nullptr);
    }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
        resetFocus = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);
    }
}