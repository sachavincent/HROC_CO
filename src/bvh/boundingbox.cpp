#include "bvh/boundingbox.hpp"
#include <limits>
#include "frustum.hpp"

glm::mat3 AxisBoundingBox::DEFAULT_TRANSFORM = glm::mat3{1, 0, 0, 0, 1, 0, 0, 0, 1};

float BoundingBox::distance(std::shared_ptr<BoundingBox> _A, std::shared_ptr<BoundingBox> _B)
{
    glm::vec3 centerA = _A.get()->getCenter();
    glm::vec3 centerB = _B.get()->getCenter();
    return glm::distance(centerA, centerB);
}


bool BoundingBox::isOnOrForwardPlan(const Plan& plan) const 
{
    // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
    glm::vec3 extends = size /2.0f;
    const float r = extends.x / 2.0f * std::abs(plan.normal.x) +
            extends.y * std::abs(plan.normal.y) + extends.z * std::abs(plan.normal.z);
    return -r <= plan.getSignedDistanceToPlan(center);
}



OrientedBoundingBox::OrientedBoundingBox(const Object &_o, glm::mat3 &_transform) : BoundingBox(_o), transform(_transform)
{
}

OrientedBoundingBox::OrientedBoundingBox(glm::vec3 _center, glm::vec3 _size) : BoundingBox(_center, _size)
{
}

AxisBoundingBox::AxisBoundingBox(glm::vec3 _center, glm::vec3 _size) : OrientedBoundingBox(_center, _size)
{
}

AxisBoundingBox::AxisBoundingBox(const Object &_o) : OrientedBoundingBox(_o, AxisBoundingBox::DEFAULT_TRANSFORM)
{
}

BoundingBox::BoundingBox(const Object &_o)
{

    glm::vec3 minPos = _o.getBounds().first;
    glm::vec3 maxPos = _o.getBounds().second;

    center =  _o.getPosition();
    size = ((maxPos-minPos));

    if (size[0] < 0 || size[1] < 0 || size[2] < 0){
        throw std::invalid_argument("Incorrect BoundingBox size: " + glm::to_string(size));
    }
        

    wireframe = std::make_shared<BoundingBoxObject>(_o.getName(), center, glm::mat4(1.0), size);
}

BoundingBox::BoundingBox(glm::vec3 _center, glm::vec3 _size)
{
    center = _center;
    size = _size;

    if (size[0] < 0 || size[1] < 0 || size[2] < 0)
        throw std::invalid_argument("Incorrect BoundingBox size: " + glm::to_string(size));

    wireframe = std::make_shared<BoundingBoxObject>("NoObject", center, glm::mat4(1.0), size);
}

BoundingBox *OrientedBoundingBox::merge(BoundingBox *_other)
{
    glm::vec3 _center = getCenter();
    glm::vec3 _centerOther = _other->getCenter();

    glm::vec3 _size = getSize();
    glm::vec3 _sizeOther = _other->getSize();

    float minX = _center[0] - _size[0] / 2;
    float maxX = _center[0] + _size[0] / 2;
    float minY = _center[1] - _size[1] / 2;
    float maxY = _center[1] + _size[1] / 2;
    float minZ = _center[2] - _size[2] / 2;
    float maxZ = _center[2] + _size[2] / 2;

    float minXOther = _centerOther[0] - _sizeOther[0] / 2;
    float maxXOther = _centerOther[0] + _sizeOther[0] / 2;
    float minYOther = _centerOther[1] - _sizeOther[1] / 2;
    float maxYOther = _centerOther[1] + _sizeOther[1] / 2;
    float minZOther = _centerOther[2] - _sizeOther[2] / 2;
    float maxZOther = _centerOther[2] + _sizeOther[2] / 2;

    float newMinX = minX < minXOther ? minX : minXOther;
    float newMaxX = maxX > maxXOther ? maxX : maxXOther;
    float newMinY = minY < minYOther ? minY : minYOther;
    float newMaxY = maxY > maxYOther ? maxY : maxYOther;
    float newMinZ = minZ < minZOther ? minZ : minZOther;
    float newMaxZ = maxZ > maxZOther ? maxZ : maxZOther;

    glm::vec3 minPos(newMinX, newMinY, newMinZ);
    glm::vec3 maxPos(newMaxX, newMaxY, newMaxZ);

    BoundingBox *newBoundingBox = new OrientedBoundingBox((maxPos + minPos) * glm::vec3(0.5f), maxPos - minPos);

    return newBoundingBox;
}

BoundingBox *AxisBoundingBox::merge(BoundingBox *_other)
{
    glm::vec3 _center = getCenter();
    glm::vec3 _centerOther = _other->getCenter();

    glm::vec3 _size = getSize();
    glm::vec3 _sizeOther = _other->getSize();

    float minX = _center[0] - _size[0] / 2;
    float maxX = _center[0] + _size[0] / 2;
    float minY = _center[1] - _size[1] / 2;
    float maxY = _center[1] + _size[1] / 2;
    float minZ = _center[2] - _size[2] / 2;
    float maxZ = _center[2] + _size[2] / 2;

    float minXOther = _centerOther[0] - _sizeOther[0] / 2;
    float maxXOther = _centerOther[0] + _sizeOther[0] / 2;
    float minYOther = _centerOther[1] - _sizeOther[1] / 2;
    float maxYOther = _centerOther[1] + _sizeOther[1] / 2;
    float minZOther = _centerOther[2] - _sizeOther[2] / 2;
    float maxZOther = _centerOther[2] + _sizeOther[2] / 2;

    float newMinX = minX < minXOther ? minX : minXOther;
    float newMaxX = maxX > maxXOther ? maxX : maxXOther;
    float newMinY = minY < minYOther ? minY : minYOther;
    float newMaxY = maxY > maxYOther ? maxY : maxYOther;
    float newMinZ = minZ < minZOther ? minZ : minZOther;
    float newMaxZ = maxZ > maxZOther ? maxZ : maxZOther;

    glm::vec3 minPos(newMinX, newMinY, newMinZ);
    glm::vec3 maxPos(newMaxX, newMaxY, newMaxZ);

    BoundingBox *newBoundingBox = new AxisBoundingBox((maxPos + minPos) * glm::vec3(0.5f), maxPos - minPos);

    return newBoundingBox;
}