#ifndef ENGINE_H
#define ENGINE_H

#include "camera.hpp"
#include "ui.hpp"
#include <string>

class Engine
{
public:
    Engine(float width, float height);

    void clear();

    inline void setCamera(Camera *camera) { _camera = camera; }

    void loadScene(const std::string& file);
    
    void loadScene();

    void loop();
private:
    Ui _ui;
    float _width;
    float _height;
    Camera* _camera;
};
#endif