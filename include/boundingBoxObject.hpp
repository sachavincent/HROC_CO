#ifndef BBOBJECT_H
#define BBOBJECT_H

#include "object.hpp"

class BoundingBoxObject : public Cube
{
private:

public:
    BoundingBoxObject(std::string _parentName, const glm::vec3 &_pos, const glm::mat4 &_rotationMatrix, const glm::vec3 &_scale);

    void draw(Scene *_scene, int _num);

    void drawQuery(Scene *_scene);
};
#endif
