#ifndef BOUNDINGBOX_HPP
#define BOUNDINGBOX_HPP
#include <stdlib.h>
#include <iostream>
#include <map>
#include <stdexcept>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include "object.hpp"

class BoundingBox
{
private:
    glm::vec3 _size;
    glm::vec3 _center;
    BoundingBoxObject *_wireframe = nullptr;

public:
    BoundingBox()
    {
        std::cout << "Wrong constructor..." << std::endl;
    };

    BoundingBox(Object &o);

    inline BoundingBox(glm::vec3 center, glm::vec3 size)
    {
        _center = center;
        _size = size;

        if (size[0] < 0 || size[1] < 0 || size[2] < 0)
            throw std::invalid_argument("Incorrect BoundingBox size: " + glm::to_string(size));

        _wireframe = new BoundingBoxObject("NoObject", _center, _size);
    }

    virtual ~BoundingBox() {}

    inline const glm::vec3 &getSize() const { return _size; }
    inline const glm::vec3 &getCenter() const { return _center; }

    BoundingBox *merge(BoundingBox *A)
    {
        return this;
    }

    inline static float distance(BoundingBox *A, BoundingBox *B)
    {
        glm::vec3 centerA = A->getCenter();
        glm::vec3 centerB = B->getCenter();
        return glm::distance(centerA, centerB);
    };

    //! Get a model of the BoundingBox object for debug mode rendering
    BoundingBoxObject *getWireframe();
};
class OrientedBoundingBox : public BoundingBox
{
private:
    glm::mat3 _transform;

public:
    OrientedBoundingBox(Object &o, glm::mat3 &transform);

    inline OrientedBoundingBox(glm::vec3 center, glm::vec3 size) : BoundingBox(center, size)
    {
    }

    ~OrientedBoundingBox() override
    {
    }

    inline const glm::mat3 &getTransform() const { return _transform; }

    BoundingBox *merge(BoundingBox *other)
    {
        glm::vec3 center = getCenter();
        glm::vec3 centerOther = other->getCenter();

        glm::vec3 newCenter = (center + centerOther) * glm::vec3(0.5);
        glm::vec3 size = getSize();
        glm::vec3 sizeOther = other->getSize();

        float minX = center[0] - size[0] / 2;
        float maxX = center[0] + size[0] / 2;
        float minY = center[1] - size[1] / 2;
        float maxY = center[1] + size[1] / 2;
        float minZ = center[2] - size[2] / 2;
        float maxZ = center[2] + size[2] / 2;

        float minXOther = centerOther[0] - sizeOther[0] / 2;
        float maxXOther = centerOther[0] + sizeOther[0] / 2;
        float minYOther = centerOther[1] - sizeOther[1] / 2;
        float maxYOther = centerOther[1] + sizeOther[1] / 2;
        float minZOther = centerOther[2] - sizeOther[2] / 2;
        float maxZOther = centerOther[2] + sizeOther[2] / 2;

        float newMinX = minX < minXOther ? minX : minXOther;
        float newMaxX = maxX > maxXOther ? maxX : maxXOther;
        float newMinY = minY < minYOther ? minY : minYOther;
        float newMaxY = maxY > maxYOther ? maxY : maxYOther;
        float newMinZ = minZ < minZOther ? minZ : minZOther;
        float newMaxZ = maxZ > maxZOther ? maxZ : maxZOther;

        glm::vec3 newSize(newMaxX - newMinX, newMaxY - newMinY, newMaxZ - newMinZ);

        BoundingBox *newBoundingBox = new OrientedBoundingBox(newCenter, newSize);

        return newBoundingBox;
    }
};

class AxisBoundingBox : public OrientedBoundingBox
{
private:
    static glm::mat3 DEFAULT_TRANSFORM;

public:
    AxisBoundingBox(Object &o);

    inline AxisBoundingBox(glm::vec3 center, glm::vec3 size) : OrientedBoundingBox(center, size)
    {
    }

    BoundingBox *merge(BoundingBox *other)
    {
        glm::vec3 center = getCenter();
        glm::vec3 centerOther = other->getCenter();

        glm::vec3 newCenter = (center + centerOther) * glm::vec3(0.5);
        glm::vec3 size = getSize();
        glm::vec3 sizeOther = other->getSize();

        float minX = center[0] - size[0] / 2;
        float maxX = center[0] + size[0] / 2;
        float minY = center[1] - size[1] / 2;
        float maxY = center[1] + size[1] / 2;
        float minZ = center[2] - size[2] / 2;
        float maxZ = center[2] + size[2] / 2;

        float minXOther = centerOther[0] - sizeOther[0] / 2;
        float maxXOther = centerOther[0] + sizeOther[0] / 2;
        float minYOther = centerOther[1] - sizeOther[1] / 2;
        float maxYOther = centerOther[1] + sizeOther[1] / 2;
        float minZOther = centerOther[2] - sizeOther[2] / 2;
        float maxZOther = centerOther[2] + sizeOther[2] / 2;

        float newMinX = minX < minXOther ? minX : minXOther;
        float newMaxX = maxX > maxXOther ? maxX : maxXOther;
        float newMinY = minY < minYOther ? minY : minYOther;
        float newMaxY = maxY > maxYOther ? maxY : maxYOther;
        float newMinZ = minZ < minZOther ? minZ : minZOther;
        float newMaxZ = maxZ > maxZOther ? maxZ : maxZOther;

        glm::vec3 newSize(newMaxX - newMinX, newMaxY - newMinY, newMaxZ - newMinZ);

        BoundingBox *newBoundingBox = new AxisBoundingBox(newCenter, newSize);

        return newBoundingBox;
    }
    ~AxisBoundingBox()
    {
    }
};
#endif