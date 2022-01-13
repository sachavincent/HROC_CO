#ifndef UI_H
#define UI_H

#include "headers.hpp"

class Scene;

class Ui{
private:
    Scene* scene;
    int lightListIndex = 0;
    int lastlightListIndex = 0;
    int modelListIndex = 0;
    bool newLightWindowActive = false;
    float nlMult;
    bool staticmultSet = false;
    bool smEnabled;

    void lightsParams();
    void objectsParams();
    void newLightWindow();
    void newPointLight();
    void newDistantLight();

public:
    Ui();
    void load(GLFWwindow* _window);
    void render(Scene* _scene);
    

};


#endif