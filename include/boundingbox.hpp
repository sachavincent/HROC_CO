#ifndef BOUNDINGBOX_HPP
#define BOUNDINGBOX_HPP
#include <stdlib.h>
#include <iostream>
#include <map>

#include "headers.hpp"

#include "object.hpp"

using namespace glm;
class BoundingBox
{
private:
    vec3 size;
    vec3 center;

public:
    BoundingBox(Object &o);
    vec3 getSize();
    vec3 getCenter();
    // abstraite
    BoundingBox merge(BoundingBox &A){return *this;};
    static float distance(BoundingBox &A, BoundingBox &B);
};

class OrientedBoundingBox : BoundingBox
{
private:
    mat3 transform;

public:
    OrientedBoundingBox(Object &o, mat3 transform);
    mat3 getTransform();
    OrientedBoundingBox merge(OrientedBoundingBox A);
};

class AxisBoundingBox : OrientedBoundingBox
{
private:
    mat3 transform = mat3{1, 0, 0, 0, 1, 0, 0, 0, 1};

public:
    AxisBoundingBox(Object &o);
};
#endif