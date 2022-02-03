#include "ioutils.hpp"
#include <iostream>

void IOUtils::framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    _resetFocus = true;
    _engine->setResolution(width, height);
}

void IOUtils::mouseCallback(GLFWwindow *window, double xpos, double ypos)
{
    if (!_resetFocus)
    {
        double xoffset = xpos - _lastMouseX;
        double yoffset = _lastMouseY - ypos;

        _lastMouseX = xpos;
        _lastMouseY = ypos;
        _engine->getCurrentCamera()->offsetYaw((float)xoffset, MOUSE_SENSITIVTY);
        _engine->getCurrentCamera()->offsetPitch((float)yoffset, MOUSE_SENSITIVTY);
    }
    else
    {
        _lastMouseX = _engine->getCurrentCamera()->getResWidth() / 2;
        _lastMouseY = _engine->getCurrentCamera()->getResHeight() / 2;
        glfwSetCursorPos(window, _lastMouseX, _lastMouseY);
        _resetFocus = false;
    }
}

void IOUtils::scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    _engine->getCurrentCamera()->offsetFov(yoffset);
}

void IOUtils::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    switch (action)
    {
    case GLFW_PRESS:
        IOUtils::onKeyPressed(window, key);
        break;
    case GLFW_RELEASE:
        IOUtils::onKeyReleased(window, key);
        break;
    default:
        // No need for now
        break;
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

    glfwSetWindowSize(window, width, height);
    glViewport(0, 0, width, height);
}

void IOUtils::onKeyPressed(GLFWwindow *window, int key)
{
    switch (key)
    {
    case GLFW_KEY_E:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    case GLFW_KEY_R:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    case GLFW_KEY_T:
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        break;

    case GLFW_KEY_W:
    case GLFW_KEY_UP:
        _engine->getCurrentCamera()->moveZ(1.0);
        break;
    case GLFW_KEY_S:
    case GLFW_KEY_DOWN:
        _engine->getCurrentCamera()->moveZ(-1.0);
        break;
    case GLFW_KEY_A:
    case GLFW_KEY_LEFT:
        _engine->getCurrentCamera()->moveX(-1.0);
        break;
    case GLFW_KEY_D:
    case GLFW_KEY_RIGHT:
        _engine->getCurrentCamera()->moveX(1.0);
        break;

    case GLFW_KEY_SPACE:
        _engine->getCurrentCamera()->moveY(1.0);
        break;
    case GLFW_KEY_LEFT_CONTROL:
        _engine->getCurrentCamera()->moveY(-1.0);
        break;

    case GLFW_KEY_U:
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetCursorPosCallback(window, nullptr);
        glfwSetScrollCallback(window, nullptr);
        break;

    case GLFW_KEY_I:
        _resetFocus = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPosCallback(window, IOUtils::mouseCallback);
        glfwSetScrollCallback(window, IOUtils::scrollCallback);
        break;

    case GLFW_KEY_L:
        _engine->switchCamera();
        break;
    default:
        // Key not assigned
        break;
    }
}

void IOUtils::onKeyReleased(GLFWwindow *window, int key)
{
    //const float cameraSpeed = 200.0f * (float)_engine->getDeltaTime(); // framerate independent
    switch (key)
    {
    case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(window, true);
        break;
    case GLFW_KEY_W:
    case GLFW_KEY_UP:
        _engine->getCurrentCamera()->moveZ(-1.0);
        break;
    case GLFW_KEY_S:
    case GLFW_KEY_DOWN:
        _engine->getCurrentCamera()->moveZ(1.0);
        break;
    case GLFW_KEY_A:
    case GLFW_KEY_LEFT:
        _engine->getCurrentCamera()->moveX(1.0);
        break;
    case GLFW_KEY_D:
    case GLFW_KEY_RIGHT:
        _engine->getCurrentCamera()->moveX(-1.0);
        break;

    case GLFW_KEY_SPACE:
        _engine->getCurrentCamera()->moveY(-1.0);
        break;
    case GLFW_KEY_LEFT_CONTROL:
        _engine->getCurrentCamera()->moveY(1.0);
        break;
    default:
        // Key not assigned
        break;
    }
}