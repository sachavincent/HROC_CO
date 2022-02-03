#ifndef ENGINE_H
#define ENGINE_H

#define WINDOW_NAME "HROC_APP"

#include <string>

#ifdef NOT_MSVC
#include <GLFW/glfw3.h>
#else
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#endif

#include "camera.hpp"
#include "ui.hpp"

class Scene;

enum CameraType
{
    FREE,
    STATIC
};
class Engine
{
public:
    Engine(float width, float height);

    ~Engine();

    void clear();

    Scene loadScene(const std::string &file);

    Scene loadScene();

    void startLoop();

    void endLoop();

    void updateFpsCounter(double _updateRateMs);

    void setResolution(int width, int height);

    inline Scene &getScene() const { return *_scene; }

    inline double getDeltaTime() const { return _deltaTime; }

    inline Camera *getFreeCam() const { return _freeCam; }

    inline Camera *getStaticCamera() const { return _camera; }

    inline Camera *getCurrentCamera() const { return _currentCamera == CameraType::STATIC ? _camera : _freeCam; }

    // This method switches between the two available cameras
    void switchCamera();

private:
    Ui _ui;
    float _width;
    float _height;
    Camera *_camera;
    Camera *_freeCam;

    CameraType _currentCamera;
    Scene *_scene;
    GLFWwindow *_window;
    std::string _windowName = WINDOW_NAME;

    double _deltaTime;
    double _lastFrame;
};
#endif