#include "bvh/bounding_box.hpp"
#include "frustum.hpp"

glm::mat3 AxisBoundingBox::DEFAULT_TRANSFORM = glm::mat3{1, 0, 0, 0, 1, 0, 0, 0, 1};

BoundingBox::BoundingBox(const glm::vec3 &v) : minV(v), maxV(v)
{
}
BoundingBox::BoundingBox(const glm::vec3 &_min, const glm::vec3 &_max) : minV(_min), maxV(_max)
{
    wireframe = std::make_shared<BoundingBoxObject>("NoObject", center(), glm::mat4(1.0), diagonal());
}

BoundingBox::BoundingBox(std::shared_ptr<Object> _o)
{
    auto [minB, maxB] = _o->getBounds();
    minV = minB;
    maxV = maxB;
    object = _o;
    wireframe = std::make_shared<BoundingBoxObject>(_o->getName(), center(), glm::mat4(1.0), diagonal());
}

bool BoundingBox::isOnOrForwardPlan(const Plan &plan) const
{
    // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
    glm::vec3 size = diagonal();
    glm::vec3 extends = size / 2.0f;
    const float r = extends.x / 2.0f * std::abs(plan.normal.x) +
                    extends.y * std::abs(plan.normal.y) + extends.z * std::abs(plan.normal.z);
    return -r <= plan.getSignedDistanceToPlan(center());
}

BoundingBox OrientedBoundingBox::merge(const BoundingBox &_other)
{
    glm::vec3 _center = center();
    glm::vec3 _centerOther = _other.center();

    glm::vec3 _size = diagonal();
    glm::vec3 _sizeOther = _other.diagonal();

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

    OrientedBoundingBox newBoundingBox(minPos, maxPos);

    return newBoundingBox;
}

BoundingBox AxisBoundingBox::merge(const BoundingBox &_other)
{
    glm::vec3 _center = center();
    glm::vec3 _centerOther = _other.center();

    glm::vec3 _size = diagonal();
    glm::vec3 _sizeOther = _other.diagonal();

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

    AxisBoundingBox newBoundingBox(minPos, maxPos);

    return newBoundingBox;
}