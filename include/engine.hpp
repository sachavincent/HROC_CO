#ifndef ENGINE_H
#define ENGINE_H

#define WINDOW_NAME "HROC_APP"

#include <string>

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

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
    Engine(float _width, float _height);

    ~Engine();

    void clear();

    void loadScene(const std::string &_file);

    void loadScene(Scene *_scene);

    void loadScene();

    void startLoop();

    void endLoop();

    void updateFpsCounter(double _updateRateMs);

    void setResolution(int width, int height);

    inline Scene *getScene() const { return scene; }

    inline double getDeltaTime() const { return deltaTime; }

    inline Camera *getFreeCam() const { return freeCam; }

    inline Camera *getStaticCamera() const { return camera; }

    inline Camera *getCurrentCamera() const { return currentCamera == CameraType::STATIC ? camera : freeCam; }
    inline const CameraType &getCurrentCameraType() const { return currentCamera; }

    inline Ui &getUi() { return ui; }

    // This method switches between the two available cameras
    void switchCamera();

    void switchPolygonMode()
    {
        polygonMode = polygonMode == GL_FILL ? GL_LINE : GL_FILL;
    }

    
    double deltaTime;

private:
    Ui ui;
    float width;
    float height;
    Camera *camera;
    Camera *freeCam;

    CameraType currentCamera;
    Scene *scene;
    GLFWwindow *window;
    std::string windowName = WINDOW_NAME;

    int polygonMode = GL_FILL;
    double lastFrame;

    bool isFirstLoop = true;
};
#endif