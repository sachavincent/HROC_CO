#include "boundingbox.hpp"

glm::mat3 AxisBoundingBox::DEFAULT_TRANSFORM = glm::mat3{1, 0, 0, 0, 1, 0, 0, 0, 1};

AxisBoundingBox::AxisBoundingBox(Object &o) : OrientedBoundingBox(o, AxisBoundingBox::DEFAULT_TRANSFORM)
{
}

OrientedBoundingBox::OrientedBoundingBox(Object &o, glm::mat3 &transform) : BoundingBox(o), _transform(transform)
{
}

BoundingBox::BoundingBox(Object &o)
{
    _center = o.getPosition();

    float maxX = 0, maxY = 0 , maxZ = 0;
    float minX = INFINITE, minY = INFINITE, minZ = INFINITE;


    for(auto it = o.getVertices().begin();it != o.getVertices().end();it++)
    {
        minX = (*it) < minX ? (*it): minX;
        maxX = (*it) > maxX ? (*it): maxX;
        it++;
        minY = (*it) < minY ? (*it): minY;
        maxY = (*it) > maxY ? (*it): maxY;
        it++;
        minZ = (*it) < minZ ? (*it): minZ;
        maxZ = (*it) > maxZ ? (*it): maxZ;
    }
    _size = glm::vec3(maxX - minX,maxY - minY,maxZ - minZ);
}

Object *BoundingBox::getWireframe()
{
    if (_wireframe)
        return _wireframe;
    _wireframe = new Cube(1.0);
    _wireframe->setPosition(_center);
    _wireframe->setScale(_size);
    return _wireframe;
}
