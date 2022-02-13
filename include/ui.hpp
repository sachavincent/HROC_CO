#ifndef UI_H
#define UI_H

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
class Scene;

class Ui
{
private:
    void lightsParams();
    void objectsParams();
    void newLightWindow();
    void newPointLight();
    void newDistantLight();

private:
    Scene *scene;
    int lightListIndex = 0;
    int lastlightListIndex = 0;
    int lastLightCount = -1;
    int objectsListIndex = 0;
    bool newLightWindowActive = false;


    int bboxMaxLevel;
    /** Mode of bbox visualisation
    // -1 = none visvible
    // 0 = all visvible
    // 1 = first level visible
    // 1 = second level visible
    // etc..
    **/

    int bboxMode = 0;
    bool objectMode = true;

public:
    Ui();
    void load(GLFWwindow *_window);
    void render(Scene *_scene);
    void setBboxMaxLevel(int _level) { bboxMaxLevel = _level; }
    int getBboxVisMode() { return bboxMode; }
    int getObjectsVisMode() { return objectMode; }
};

#endif