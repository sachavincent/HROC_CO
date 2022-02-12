#include "boundingbox.hpp"
#include <limits>

glm::mat3 AxisBoundingBox::DEFAULT_TRANSFORM = glm::mat3{1, 0, 0, 0, 1, 0, 0, 0, 1};

float BoundingBox::distance(std::shared_ptr<BoundingBox> _A, std::shared_ptr<BoundingBox> _B)
{
    glm::vec3 centerA = _A.get()->getCenter();
    glm::vec3 centerB = _B.get()->getCenter();
    return glm::distance(centerA, centerB);
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
    float maxX = -std::numeric_limits<float>::infinity(), maxY = -std::numeric_limits<float>::infinity(), maxZ = -std::numeric_limits<float>::infinity();
    float minX = std::numeric_limits<float>::infinity(), minY = std::numeric_limits<float>::infinity(), minZ = std::numeric_limits<float>::infinity();

    for (auto it = _o.getVertices().begin(); it != _o.getVertices().end();)
    {
        GLfloat x = *it;
        it++;
        GLfloat y = *it;
        it++;
        GLfloat z = *it;
        it++;

        glm::vec3 pos(x, y, z);
        pos = glm::vec3(_o.getTransformationMatrix() * glm::vec4(pos, 1.0)); // Apply object rotations & scale

        minX = pos[0] < minX ? pos[0] : minX;
        maxX = pos[0] > maxX ? pos[0] : maxX;

        minY = pos[1] < minY ? pos[1] : minY;
        maxY = pos[1] > maxY ? pos[1] : maxY;

        minZ = pos[2] < minZ ? pos[2] : minZ;
        maxZ = pos[2] > maxZ ? pos[2] : maxZ;
    }

    glm::vec3 minPos(minX, minY, minZ);
    glm::vec3 maxPos(maxX, maxY, maxZ);

    center = (maxPos + minPos) * glm::vec3(0.5f);
    size = maxPos - minPos;
    if (size[0] < 0 || size[1] < 0 || size[2] < 0)
        throw std::invalid_argument("Incorrect BoundingBox size: " + glm::to_string(size));

    wireframe = new BoundingBoxObject(_o.getName(), center, glm::mat4(1.0), size);
}

BoundingBox::BoundingBox(glm::vec3 _center, glm::vec3 _size)
{
    center = _center;
    size = _size;

    if (size[0] < 0 || size[1] < 0 || size[2] < 0)
        throw std::invalid_argument("Incorrect BoundingBox size: " + glm::to_string(size));

    wireframe = new BoundingBoxObject("NoObject", center, glm::mat4(1.0), size);
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
    glm::vec3 newSize(newMaxX - newMinX, newMaxY - newMinY, newMaxZ - newMinZ);
    glm::vec3 newCenter((newMaxX + newMinX) / 2, (newMaxY + newMinY) / 2, (newMaxZ + newMinZ) / 2);

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