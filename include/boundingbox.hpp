#ifndef BOUNDINGBOX_HPP
#define BOUNDINGBOX_HPP
#include <stdlib.h>
#include <iostream>
#include <map>

#include <glm/glm.hpp>
#include "object.hpp"

class BoundingBox
{
private:
    glm::vec3 _size;
    glm::vec3 _center;

public:
    BoundingBox(){};
    BoundingBox(Object &o);
    virtual ~BoundingBox() = 0;

    inline const glm::vec3 &getSize() const { return _size; }
    inline const glm::vec3 &getCenter() const { return _center; }

    virtual const BoundingBox *merge(const BoundingBox &A) const = 0;

    inline static float distance(const BoundingBox &A, const BoundingBox &B){ 
        glm::vec3 centerA = A.getCenter();
        glm::vec3 centerB = B.getCenter();
        return  glm::distance(centerA,centerB);
    };
};
class OrientedBoundingBox : public BoundingBox
{
private:
    glm::mat3 _transform;

public:
    OrientedBoundingBox(Object &o, glm::mat3 &transform);
    ~OrientedBoundingBox() override = 0;
    inline const glm::mat3 &getTransform() const { return _transform; }

    const BoundingBox *merge(const BoundingBox &A) const override;
};

class AxisBoundingBox : public OrientedBoundingBox
{
private:
    static glm::mat3 DEFAULT_TRANSFORM;

public:
    AxisBoundingBox(Object &o);
    ~AxisBoundingBox() override = 0;
};
#endif