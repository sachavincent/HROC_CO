#include "utils/ioutils.hpp"
#include <iostream>

void IOUtils::framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    resetFocus = true;
    engine->setResolution(width, height);
}

void IOUtils::mouseCallback(GLFWwindow *window, double xpos, double ypos)
{
    if (!resetFocus)
    {
        double xoffset = xpos - lastMouseX;
        double yoffset = lastMouseY - ypos;

        lastMouseX = xpos;
        lastMouseY = ypos;

        engine->getCurrentCamera()->offsetYaw((float)xoffset, MOUSE_SENSITIVTY);
        engine->getCurrentCamera()->offsetPitch((float)yoffset, MOUSE_SENSITIVTY);
    }
    else
    {
        lastMouseX = engine->getCurrentCamera()->getResWidth() / 2;
        lastMouseY = engine->getCurrentCamera()->getResHeight() / 2;
        glfwSetCursorPos(window, lastMouseX, lastMouseY);
        resetFocus = false;
    }
}

void IOUtils::scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    engine->getCurrentCamera()->offsetFov(yoffset);
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
    if (!engine)
    {
        std::cerr << "Unknown engine pointer!" << std::endl;
        return;
    }

    engine->setResolution(width, height);

    glfwSetWindowSize(window, width, height);
    glViewport(0, 0, width, height);
}

void IOUtils::onKeyPressed(GLFWwindow *window, int key)
{
    switch (key)
    {
    case GLFW_KEY_P:
        engine->switchPolygonMode();
        break;

    case GLFW_KEY_W:
    case GLFW_KEY_UP:
        engine->getCurrentCamera()->moveZ(1.0);
        break;
    case GLFW_KEY_S:
    case GLFW_KEY_DOWN:
        engine->getCurrentCamera()->moveZ(-1.0);
        break;
    case GLFW_KEY_A:
    case GLFW_KEY_LEFT:
        engine->getCurrentCamera()->moveX(-1.0);
        break;
    case GLFW_KEY_D:
    case GLFW_KEY_RIGHT:
        engine->getCurrentCamera()->moveX(1.0);
        break;
    case GLFW_KEY_SPACE:
        engine->getCurrentCamera()->moveY(1.0);
        break;
    case GLFW_KEY_LEFT_CONTROL:
        engine->getCurrentCamera()->moveY(-1.0);
        break;
    case GLFW_KEY_U:
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetCursorPosCallback(window, nullptr);
        glfwSetScrollCallback(window, nullptr);
        break;
    case GLFW_KEY_I:
        resetFocus = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPosCallback(window, IOUtils::mouseCallback);
        glfwSetScrollCallback(window, IOUtils::scrollCallback);
        break;
    case GLFW_KEY_L:
        engine->switchCamera();
        break;
    default:
        // Key not assigned
        break;
    }
}

void IOUtils::onKeyReleased(GLFWwindow *window, int key)
{
    // const float cameraSpeed = 200.0f * (float)_engine->getDeltaTime(); // framerate independent
    switch (key)
    {
    case GLFW_KEY_ESCAPE:
        engine->endLoop();
        break;
    case GLFW_KEY_W:
    case GLFW_KEY_UP:
        engine->getCurrentCamera()->moveZ(-1.0);
        break;
    case GLFW_KEY_S:
    case GLFW_KEY_DOWN:
        engine->getCurrentCamera()->moveZ(1.0);
        break;
    case GLFW_KEY_A:
    case GLFW_KEY_LEFT:
        engine->getCurrentCamera()->moveX(1.0);
        break;
    case GLFW_KEY_D:
    case GLFW_KEY_RIGHT:
        engine->getCurrentCamera()->moveX(-1.0);
        break;
    case GLFW_KEY_SPACE:
        engine->getCurrentCamera()->moveY(-1.0);
        break;
    case GLFW_KEY_LEFT_CONTROL:
        engine->getCurrentCamera()->moveY(1.0);
        break;
    default:
        // Key not assigned
        break;
    }
}