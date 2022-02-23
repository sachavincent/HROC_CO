#ifndef UI_H
#define UI_H

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <imgui.h>
//#include <implot/implot.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <ImGuiFileDialog.h>
#include <implot.h>

#include "utils/sceneBuilder.hpp"

class Scene;
class Engine;
class Ui
{
private:
  void displayParams();
  void lightsParams();
  void sceneLoading();
  void benchmarkParams();

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

  int bboxMode = -1;
  bool frustumMode = true;
  bool frustumOutlineMode = true;
  bool objectMode = true;
  bool occludeeColorMode = false;

  // pipelines

  bool firstEarlyZMode = true;
  bool extractOccludeesMode = true;
  bool viewFrustumCullingMode = true;
  bool secondEarlyZMode = true;
  bool batchOcclusionMode = true;

  // Cache

  bool firstEarlyZModeCache = firstEarlyZMode;
  bool extractOccludeesModeCache = extractOccludeesMode;
  bool viewFrustumCullingModeCache = viewFrustumCullingMode;
  bool secondEarlyZModeCache = secondEarlyZMode;
  bool batchOcclusionModeCache = batchOcclusionMode;

public:
  Ui();
  void load(GLFWwindow *_window, Engine *_engine);
  void render();
  void setBboxMaxLevel(int _level) { bboxMaxLevel = _level; }
  int getBboxVisMode() { return bboxMode; }
  bool getFrustumVisMode() { return frustumMode; }
  bool getFrustumOutlineVisMode() { return frustumOutlineMode; }
  bool getObjectsVisMode() { return objectMode; }
  bool getOccludeeColorMode() { return occludeeColorMode; }

  bool getFirstEarlyZMode() { return firstEarlyZMode; }
  bool getExtractOccludeesMode() { return extractOccludeesMode; }
  bool getVFCMode() { return viewFrustumCullingMode; }
  bool getSecondEarlyZMode() { return secondEarlyZMode; }
  bool getBatchOcclusionMode() { return batchOcclusionMode; }

  bool &getFirstEarlyZModeCache() { return firstEarlyZModeCache; }
  bool &getExtractOccludeesModeCache() { return extractOccludeesModeCache; }
  bool &getVFCModeCache() { return viewFrustumCullingModeCache; }
  bool &getSecondEarlyZModeCache() { return secondEarlyZModeCache; }
  bool &getBatchOcclusionModeCache() { return batchOcclusionModeCache; }
  
  void setPipelineMode(bool _mode)
  {
    batchOcclusionMode = firstEarlyZMode = extractOccludeesMode = viewFrustumCullingMode = secondEarlyZMode = _mode;
  }
};

#endif