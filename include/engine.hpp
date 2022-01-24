#ifndef ENGINE_H
#define ENGINE_H

#define WINDOW_NAME "HROC_APP"

#include <string>
#include <GLFW/glfw3.h>

#include "camera.hpp"
#include "ui.hpp"

class Scene;

class Engine
{
public:
    Engine(float width, float height);

    ~Engine();

    void clear();

    inline void setCamera(Camera *camera) { _camera = camera; }

    Scene loadScene(const std::string &file);

    Scene loadScene();

    void startLoop();

    void endLoop();

    void updateFpsCounter(double _updateRateMs);

    void setResolution(int width, int height);

    inline Scene &getScene() const { return *_scene; }

private:
    Ui _ui;
    float _width;
    float _height;
    Camera *_camera;
    Scene *_scene;
    GLFWwindow *_window;
    std::string _windowName = WINDOW_NAME;

    double _deltaTime;
    double _lastFrame;
};
#endif