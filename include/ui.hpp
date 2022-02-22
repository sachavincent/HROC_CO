#ifndef UI_H
#define UI_H

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <imgui.h>
//#include <implot/implot.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <implot.h>
#include <ImGuiFileDialog.h>

#include "utils/sceneBuilder.hpp"

class Scene;
class Engine;
class Ui
{
private:
    void displayParams();
    void lightsParams();
    void sceneLoading();

    void newSceneWindow();
    void newLightWindow();
    void newPointLight();
    void newDistantLight();
    void plotTimer();
    void plotFpsRate();

    void displayPipelineOptions();

private:
    Engine *engine;
    int lightListIndex = 0;
    int lastlightListIndex = 0;
    int lastLightCount = -1;
    int objectsListIndex = 0;
    bool newLightWindowActive = false;

    int bboxMaxLevel;
    /** Mode of bbox visualisation
    // -1 = none visible
    // 0 = all visible
    // 1 = first level visible
    // 1 = second level visible
    // etc..
    **/

    int bboxMode = 0;
    bool frustumMode = true;
    bool frustumOutlineMode = true;
    bool objectMode = true;

    // pipelines

    bool firstEarlyZMode = true;
    bool extractOccludeesMode = true;
    bool viewFrustumCullingMode = true;
    bool secondEarlyZMode = true;

public:
    Ui();
    void load(GLFWwindow *_window, Engine *_engine);
    void render();
    void setBboxMaxLevel(int _level) { bboxMaxLevel = _level; }
    int getBboxVisMode() { return bboxMode; }
    int getFrustumVisMode() { return frustumMode; }
    int getFrustumOutlineVisMode() { return frustumOutlineMode; }
    int getObjectsVisMode() { return objectMode; }
    int getFirstEarlyZMode() { return firstEarlyZMode; }
    int getSecondEarlyZMode() { return secondEarlyZMode; }
    int getVFCMode() { return viewFrustumCullingMode; }

    int getExtractOccludeesMode() { return extractOccludeesMode; }
};

#endif