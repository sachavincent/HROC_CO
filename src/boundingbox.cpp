#include "boundingbox.hpp"
#include <limits>

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

    float maxX = -std::numeric_limits<float>::infinity(), maxY = -std::numeric_limits<float>::infinity(), maxZ = -std::numeric_limits<float>::infinity();
    float minX = std::numeric_limits<float>::infinity(), minY = std::numeric_limits<float>::infinity(), minZ = std::numeric_limits<float>::infinity();

    for (auto it = o.getVertices().begin(); it != o.getVertices().end(); it++)
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
    _size = glm::vec3(maxX - minX, maxY - minY, maxZ - minZ);
    // TODO: Transformation matrix (scale+trans+rot)

    if (_size[0] < 0 || _size[1] < 0 || _size[2] < 0)
        throw std::invalid_argument("Incorrect BoundingBox size: " + glm::to_string(_size));

    _wireframe = new BoundingBoxObject(o.getName(), _center, _size);
}
