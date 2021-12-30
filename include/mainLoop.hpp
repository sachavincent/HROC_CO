#ifndef MAINLOOP_H
#define MAINLOOP_H

#include <chrono>

#include "scene.hpp"
#include "ui.hpp"
#include "inputs.hpp"
#include "headers.hpp"

//! The render loop of the app, setups the window and UI to render a given scene. 
class MainLoop {

private:
    Scene& scene;
    Ui ui;
    Camera& cam;
    Inputs inputs;

    GLFWwindow* window;

    // default window size
    int windowWidth=800;
    int windowHeight=600;
    double deltaTime;
    double lastFrame;

    std::string windowName = "HROC App";

    void updateFpsCounter(uint32_t _updateRateMs);

public:
    /**
     *  Starts the render procedure 
     * \param _scene The scene object we want to render
     * \param _ui The ImGUI IU to render on screen
     * \param _camera The camera form which we view the scene
     **/
    MainLoop(Scene& _scene, Ui& _ui, Camera& _camera);
    ~MainLoop();
    //! Start the render loop
    void run();
};


#endif