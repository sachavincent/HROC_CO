#include "ioutils.hpp"
#include <iostream>

void IOUtils::framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    _resetFocus = true;
    _camera->setResolution(width, height);
}

void IOUtils::mouseCallback(GLFWwindow *window, double xpos, double ypos)
{
    if (!_resetFocus)
    {
        double xoffset = xpos - _lastMouseX;
        double yoffset = _lastMouseY - ypos;

        _lastMouseX = xpos;
        _lastMouseY = ypos;
        _camera->offsetYaw(xoffset, MOUSE_SENSITIVTY);
        _camera->offsetPitch(yoffset, MOUSE_SENSITIVTY);
    }
    else
    {
        _lastMouseX = _camera->getResWidth() / 2;
        _lastMouseX = _camera->getResHeight() / 2;
        glfwSetCursorPos(window, _lastMouseX, _lastMouseY);
        _resetFocus = false;
    }
}

void IOUtils::scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    _camera->offsetFov(yoffset);
}

// manage IOUtils
void IOUtils::processInput(GLFWwindow *window, double deltaTime, Camera *camera)
{
    _camera = camera;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    // keys 1,2,3 to switch polygon display mode
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

    // _camera movement
    const float _cameraSpeed = 2.0f * (float)deltaTime; // framerate independent

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        _camera->moveZ(_cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        _camera->moveZ(-_cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        _camera->moveX(-_cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        _camera->moveX(_cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        _camera->moveZ(_cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        _camera->moveZ(-_cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        _camera->moveX(-_cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        _camera->moveX(_cameraSpeed);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        _camera->moveY(_cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        _camera->moveY(-_cameraSpeed);

    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetCursorPosCallback(window, nullptr);
        glfwSetScrollCallback(window, nullptr);
    }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
    {
        _resetFocus = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPosCallback(window, IOUtils::mouseCallback);
        glfwSetScrollCallback(window, IOUtils::scrollCallback);
    }
}

void IOUtils::updateScreenRes(GLFWwindow *window, int width, int height)
{
    if (!_engine)
    {
        std::cerr << "Unknown engine pointer!" << std::endl;
        return;
    }

    _engine->setResolution(width, height);

    _camera->setResolution(width, height);

    glfwSetWindowSize(window, width, height);
    glViewport(0, 0, width, height);
}