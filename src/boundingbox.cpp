#include "boundingbox.hpp"
#include <limits>

glm::mat3 AxisBoundingBox::DEFAULT_TRANSFORM = glm::mat3{1, 0, 0, 0, 1, 0, 0, 0, 1};

BoundingBox::BoundingBox() : wireframe(nullptr)
{
    // TODO: For test purposes
}

float BoundingBox::distance(BoundingBox *_A, BoundingBox *_B)
{
    glm::vec3 centerA = _A->getCenter();
    glm::vec3 centerB = _B->getCenter();
    return glm::distance(centerA, centerB);
}

OrientedBoundingBox::OrientedBoundingBox(Object &_o, glm::mat3 &_transform) : BoundingBox(_o), transform(_transform)
{
}

OrientedBoundingBox::OrientedBoundingBox(glm::vec3 _center, glm::vec3 _size) : BoundingBox(_center, _size)
{
}

AxisBoundingBox::AxisBoundingBox(glm::vec3 _center, glm::vec3 _size) : OrientedBoundingBox(_center, _size)
{
}

AxisBoundingBox::AxisBoundingBox(Object &_o) : OrientedBoundingBox(_o, AxisBoundingBox::DEFAULT_TRANSFORM)
{
}

BoundingBox::BoundingBox(Object &_o) : wireframe(nullptr)
{
    center = _o.getPosition();

    float maxX = -std::numeric_limits<float>::infinity(), maxY = -std::numeric_limits<float>::infinity(), maxZ = -std::numeric_limits<float>::infinity();
    float minX = std::numeric_limits<float>::infinity(), minY = std::numeric_limits<float>::infinity(), minZ = std::numeric_limits<float>::infinity();

    for (auto it = _o.getVertices().begin(); it != _o.getVertices().end(); it++)
    {
        minX = (*it) < minX ? (*it) : minX;
        maxX = (*it) > maxX ? (*it) : maxX;
        it++;
        minY = (*it) < minY ? (*it) : minY;
        maxY = (*it) > maxY ? (*it) : maxY;
        it++;
        minZ = (*it) < minZ ? (*it) : minZ;
        maxZ = (*it) > maxZ ? (*it) : maxZ;
    }
    size = glm::vec3(maxX - minX, maxY - minY, maxZ - minZ);
    // TODO: Transformation matrix (scale+trans+rot)

    if (size[0] < 0 || size[1] < 0 || size[2] < 0)
        throw std::invalid_argument("Incorrect BoundingBox size: " + glm::to_string(size));

    wireframe = new BoundingBoxObject(_o.getName(), center, size);
}

BoundingBox::BoundingBox(glm::vec3 _center, glm::vec3 _size) : wireframe(nullptr)
{
    center = _center;
    size = _size;

    if (size[0] < 0 || size[1] < 0 || size[2] < 0)
        throw std::invalid_argument("Incorrect BoundingBox size: " + glm::to_string(size));

    wireframe = new BoundingBoxObject("NoObject", center, size);
}

BoundingBox *OrientedBoundingBox::merge(BoundingBox *_other)
{
    glm::vec3 _center = getCenter();
    glm::vec3 _centerOther = _other->getCenter();

    glm::vec3 _newCenter = (_center + _centerOther) * glm::vec3(0.5);
    glm::vec3 _size = getSize();
    glm::vec3 _sizeOther = _other->getSize();

    float _minX = _center[0] - _size[0] / 2;
    float _maxX = _center[0] + _size[0] / 2;
    float _minY = _center[1] - _size[1] / 2;
    float _maxY = _center[1] + _size[1] / 2;
    float _minZ = _center[2] - _size[2] / 2;
    float _maxZ = _center[2] + _size[2] / 2;

    float _minXOther = _centerOther[0] - _sizeOther[0] / 2;
    float _maxXOther = _centerOther[0] + _sizeOther[0] / 2;
    float _minYOther = _centerOther[1] - _sizeOther[1] / 2;
    float _maxYOther = _centerOther[1] + _sizeOther[1] / 2;
    float _minZOther = _centerOther[2] - _sizeOther[2] / 2;
    float _maxZOther = _centerOther[2] + _sizeOther[2] / 2;

    float _newMinX = _minX < _minXOther ? _minX : _minXOther;
    float _newMaxX = _maxX > _maxXOther ? _maxX : _maxXOther;
    float _newMinY = _minY < _minYOther ? _minY : _minYOther;
    float _newMaxY = _maxY > _maxYOther ? _maxY : _maxYOther;
    float _newMinZ = _minZ < _minZOther ? _minZ : _minZOther;
    float _newMaxZ = _maxZ > _maxZOther ? _maxZ : _maxZOther;

    glm::vec3 _newSize(_newMaxX - _newMinX, _newMaxY - _newMinY, _newMaxZ - _newMinZ);

    BoundingBox *_newBoundingBox = new OrientedBoundingBox(_newCenter, _newSize);

    return _newBoundingBox;
}

BoundingBox *AxisBoundingBox::merge(BoundingBox *_other)
{
    glm::vec3 _center = getCenter();
    glm::vec3 _centerOther = _other->getCenter();

    glm::vec3 _newCenter = (_center + _centerOther) * glm::vec3(0.5);
    glm::vec3 _size = getSize();
    glm::vec3 _sizeOther = _other->getSize();

    float _minX = _center[0] - _size[0] / 2;
    float _maxX = _center[0] + _size[0] / 2;
    float _minY = _center[1] - _size[1] / 2;
    float _maxY = _center[1] + _size[1] / 2;
    float _minZ = _center[2] - _size[2] / 2;
    float _maxZ = _center[2] + _size[2] / 2;

    float _minXOther = _centerOther[0] - _sizeOther[0] / 2;
    float _maxXOther = _centerOther[0] + _sizeOther[0] / 2;
    float _minYOther = _centerOther[1] - _sizeOther[1] / 2;
    float _maxYOther = _centerOther[1] + _sizeOther[1] / 2;
    float _minZOther = _centerOther[2] - _sizeOther[2] / 2;
    float _maxZOther = _centerOther[2] + _sizeOther[2] / 2;

    float _newMinX = _minX < _minXOther ? _minX : _minXOther;
    float _newMaxX = _maxX > _maxXOther ? _maxX : _maxXOther;
    float _newMinY = _minY < _minYOther ? _minY : _minYOther;
    float _newMaxY = _maxY > _maxYOther ? _maxY : _maxYOther;
    float _newMinZ = _minZ < _minZOther ? _minZ : _minZOther;
    float _newMaxZ = _maxZ > _maxZOther ? _maxZ : _maxZOther;

    glm::vec3 _newSize(_newMaxX - _newMinX, _newMaxY - _newMinY, _newMaxZ - _newMinZ);

    BoundingBox *_newBoundingBox = new AxisBoundingBox(_newCenter, _newSize);

    return _newBoundingBox;
}