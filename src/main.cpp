#include "utils.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

static void OnWindowResize(GLFWwindow* window, int width, int height)
{
    AppState* app = static_cast<AppState*>(glfwGetWindowUserPointer(window));
    assert(app);
    WIDTH = width;
    HEIGHT = height;

    glViewport(0, 0, width, height);
}

static void OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
    AppState* app = static_cast<AppState*>(glfwGetWindowUserPointer(window));
    assert(app);
    app->camera.on_mouse_move(float(xpos), float(ypos));
}

static void OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
    (void)xoffset;
    AppState* app = static_cast<AppState*>(glfwGetWindowUserPointer(window));
    assert(app);
    app->camera.on_mouse_scroll(float(yoffset));
}

static void OnKeyFun(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    
}

static void HandleInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
        return;
    }

    AppState* app = static_cast<AppState*>(glfwGetWindowUserPointer(window));
    assert(app);
    FreeCamera& camera = app->camera;
    Mode& mode = app->mode;

    const struct KeyDelta
    {
        int key;
        glm::vec3 delta;
    };
    KeyDelta key_delta[] =
    {
        {GLFW_KEY_W, +camera._front},
        {GLFW_KEY_S, -camera._front},
        {GLFW_KEY_D, +camera._right},
        {GLFW_KEY_A, -camera._right},
        {GLFW_KEY_E, +camera._up},
        {GLFW_KEY_Q, -camera._up},
    };
    for (KeyDelta keyDelta : key_delta)
    {
        if (glfwGetKey(window, keyDelta.key) == GLFW_PRESS)
        {
            camera.on_keyboard_move(keyDelta.delta);
            break;
        }
    }
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    AppState app;
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "HROC", nullptr, nullptr);
    assert(window && "Failed to create window.");
    glfwSetWindowUserPointer(window, &app);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, OnWindowResize);
    glfwSetCursorPosCallback(window, OnMouseMove);
    glfwSetScrollCallback(window, OnMouseScroll);
    glfwSetKeyCallback(window, OnKeyFun);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    const int glad_ok = gladLoadGL();
    assert(glad_ok > 0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    app.camera.force_refresh();

    while (!glfwWindowShouldClose(window))
    {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
}