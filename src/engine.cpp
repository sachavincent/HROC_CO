#include "engine.hpp"
#include "scene.hpp"
#include "ioutils.hpp"

#include <chrono>
#include <sstream>
#include <mutex>
#include <iomanip>

Engine::Engine(float width, float height) : _width(width), _height(height), _scene(nullptr),
                                            _camera(new Camera(width, height)), _freeCam(new Camera(width, height)), _currentCamera(CameraType::STATIC)
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
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

    _window = glfwCreateWindow(_width, _height, _windowName.c_str(), nullptr, nullptr);
    glfwMakeContextCurrent(_window);
    glfwSwapInterval(0);
    // Callbacks binding
    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(_window, IOUtils::mouseCallback);
    glfwSetKeyCallback(_window, IOUtils::keyCallback);
    glfwSetScrollCallback(_window, IOUtils::scrollCallback);
    glfwSetFramebufferSizeCallback(_window, IOUtils::framebufferSizeCallback);
    glfwSetWindowSizeCallback(_window, IOUtils::updateScreenRes);
    IOUtils::setEngine(*this);
    // GLAD
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glViewport(0, 0, _width, _height);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ImGui Setup
    _ui.load(_window);

    _deltaTime = 0.0;
    _lastFrame = 0.0;
}

Engine::~Engine()
{
    clear();
    delete _camera;
    delete _freeCam;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}

void Engine::clear()
{
    // TODO:
    //  unload()
    delete _scene;
}

Scene Engine::loadScene(const std::string &file)
{
    if (_scene)
    {
        delete _scene;
    }
    _scene = new Scene(this, file);

    return *_scene;
}

Scene Engine::loadScene()
{
    if (_scene)
    {
        clear();
    }
    _scene = new Scene(this);

    return *_scene;
}

void Engine::startLoop()
{
    while (!glfwWindowShouldClose(_window))
    {
        double currentFrame = glfwGetTime();
        _deltaTime = currentFrame - _lastFrame;
        _lastFrame = currentFrame;

        glClearColor(0.2f, 0.2f, 0.2f, 0.2f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        updateFpsCounter(500);

        getCurrentCamera()->move(2.0f * _deltaTime);
        // final rendering of scene

        _scene->renderObjects();

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(4.0);
        _scene->renderBoundingBoxes();
        glLineWidth(1.0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // imgui part
        _ui.render(_scene);

        glfwSwapBuffers(_window);
        glfwPollEvents();
    }
}

void Engine::endLoop()
{
    glfwSetWindowShouldClose(_window, true);
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
        sstr << _windowName << "  |  FPS : " << std::fixed << std::setprecision(1) << 1 / _deltaTime;

        glfwSetWindowTitle(_window, sstr.str().c_str());
    }
    counter += _deltaTime;
}

void Engine::setResolution(int width, int height)
{
    _width = width;
    _height = height;

    _camera->setResolution(width, height);
    _freeCam->setResolution(width, height);
}

void Engine::switchCamera()
{
    _currentCamera = _currentCamera == CameraType::STATIC ? CameraType::FREE : CameraType::STATIC;
}