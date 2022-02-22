#include "engine.hpp"
#include "scene.hpp"
#include "utils/ioutils.hpp"

#include <chrono>
#include <sstream>
#include <mutex>
#include <iomanip>

Engine::Engine(float _width, float _height) : width(_width), height(_height), scene(nullptr), camera(new Camera(width, height)),
                                              freeCam(new Camera(_width, _height)), currentCamera(CameraType::STATIC)
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE); // TODO: Enlever (performances)
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
#ifdef HROC_TESTS
    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
//#elif WIN32 && _DEBUG
//  glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
#endif
    glfwSetErrorCallback(IOUtils::errorCallback); // TODO: Enlever (performances)

    window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);
    // Callbacks binding
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, IOUtils::mouseCallback);
    glfwSetKeyCallback(window, IOUtils::keyCallback);
    glfwSetScrollCallback(window, IOUtils::scrollCallback);
    glfwSetFramebufferSizeCallback(window, IOUtils::framebufferSizeCallback);
    glfwSetWindowSizeCallback(window, IOUtils::updateScreenRes);
    IOUtils::setEngine(*this);
    // GLAD
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ImGui Setup
    ui.load(window, this);

    deltaTime = 0.0;
    lastFrame = 0.0;
}

Engine::~Engine()
{
    clear();
    delete camera;
    delete freeCam;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}

void Engine::clear()
{
    delete scene;
}

void Engine::loadScene(const std::string &file)
{
    if (scene)
    {
        delete scene;
    }
    scene = new Scene(this, file);
}

void Engine::loadScene()
{
    if (scene)
    {
        delete scene;
    }
    scene = new Scene(this);
}

void Engine::loadScene(Scene *_scene)
{
    _scene->load();
    scene = _scene;
}

void Engine::startLoop()
{
    while (!glfwWindowShouldClose(window))
    {
        double _currentFrame = glfwGetTime();
        deltaTime = _currentFrame - lastFrame;
        lastFrame = _currentFrame;

        glClearColor(0.2f, 0.2f, 0.2f, 0.2f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        updateFpsCounter(500);

        getCurrentCamera()->move(2.0f * deltaTime);

        scene->updateBvh();
        scene->renderObjects();
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDisable(GL_DEPTH_TEST);
        scene->renderBoundingBoxes();
        scene->renderFrustum(true);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        scene->renderFrustum(false);

        glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

        glEnable(GL_DEPTH_TEST);
        // imgui part
        ui.render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void Engine::endLoop()
{
    glfwSetWindowShouldClose(window, true);
}

//! Shows fps on window title, update rate in ms
void Engine::updateFpsCounter(double _updateRateMs)
{
    using namespace std::chrono;
    static double counter;
    static std::once_flag flag;
    std::call_once(
        flag, [](double &c)
        { c = 1.0; },
        counter);

    if (counter >= _updateRateMs / 1000)
    {
        counter = 0;
        std::stringstream sstr;
        sstr << windowName << "  |  FPS : " << std::fixed << std::setprecision(1) << 1 / deltaTime;
        glfwSetWindowTitle(window, sstr.str().c_str());
    }
    counter += deltaTime;
}

void Engine::setResolution(int _width, int _height)
{
    width = _width;
    height = _height;

    camera->setResolution(width, height);
    freeCam->setResolution(width, height);
}

void Engine::switchCamera()
{
    currentCamera = currentCamera == CameraType::STATIC ? CameraType::FREE : CameraType::STATIC;
}