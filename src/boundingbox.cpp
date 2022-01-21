#include "boundingbox.hpp"
#include "object.hpp"

glm::mat3 AxisBoundingBox::DEFAULT_TRANSFORM = glm::mat3{1, 0, 0, 0, 1, 0, 0, 0, 1};

AxisBoundingBox::AxisBoundingBox(Object &o) : OrientedBoundingBox(o, AxisBoundingBox::DEFAULT_TRANSFORM)
{
}

OrientedBoundingBox::OrientedBoundingBox(Object &o, glm::mat3 &transform) : BoundingBox(o), _transform(transform)
{
}

BoundingBox::BoundingBox(Object &o)
{
}

const BoundingBox *OrientedBoundingBox::merge(const BoundingBox &A) const
{
    //TODO
    return &A;
}