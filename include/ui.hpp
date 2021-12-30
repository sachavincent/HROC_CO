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
    bool SSAOstatus = true;

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

// scenes exemple :
// - scene avec grid de lights et tous les objets
// - scene avec 1 light + spheres PHONG
// - scene avec 1 light + spheres PBR
// - scene avec tessellation


#endif