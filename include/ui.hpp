#ifndef UI_H
#define UI_H

#include <GLFW/glfw3.h>
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
    int objectsListIndex = 0;
    bool newLightWindowActive = false;
    float nlMult;
    bool staticmultSet = false;
    bool smEnabled;
    
public:
    Ui();
    void load(GLFWwindow *_window);
    void render(Scene *_scene);
};

#endif