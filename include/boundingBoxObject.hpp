#ifndef BBOBJECT_H
#define BBOBJECT_H

#include "object.hpp"

class BoundingBoxObject : public Cube
{
private:
    glm::vec3 pos;

public:
    BoundingBoxObject(std::string _parentName, glm::vec3 center, glm::vec3 size, float _edgeSize = 1.0f);

    void draw(Scene *_scene, int _depth);
};
#endif
