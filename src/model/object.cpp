#include "object.hpp"

#include <algorithm>

#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"
#include "scene.hpp"

size_t Object::id_counter = 0;

Object::~Object()
{
    if (boundingBox)
        boundingBox.reset();
}

Object &Object::setScale(const glm::vec3 &_scale)
{
    scale = glm::mat4{1.0};
    scale = glm::scale(scale, _scale);
    return *this;
}

Object &Object::setRotation(float _angle, glm::vec3 _axis)
{
    rotation = glm::mat4(1.0);
    rotation = glm::rotate(rotation, glm::radians(_angle), _axis);
    return *this;
}

Object &Object::setRotationMatrix(const glm::mat4 &_rotationMatrix)
{
    rotation = _rotationMatrix;
    return *this;
}

Object &Object::setPosition(const glm::vec3 &_pos)
{
    position = glm::mat4{1.0};
    position = glm::translate(position, _pos);
    return *this;
}

Object &Object::setTexDiffuse(std::string _path)
{
    Texture::loadTexture(_path, id);
    return *this;
}

Object &Object::setTexSpecular(std::string _path)
{
    // Texture::loadTexture(_path);
    return *this;
}

Object &Object::setDiffuse(glm::vec3 _color)
{
    // Texture::loadTextureColor(_color);
    diffuse = _color;
    return *this;
}

Object &Object::setSpecular(glm::vec3 _color)
{
    // specularColor = _color;
    return *this;
}

std::pair<glm::vec3, glm::vec3> Object::getBounds() const
{
    OBJECT_BOUNDS bounds = meshfilter.getObjectBounds();
    glm::mat4 tmat = rotation * scale;
    // get 8 transformed corners
    std::vector<glm::vec4> boundsCorners = {
        {tmat * glm::vec4{bounds.max.x, bounds.max.y, bounds.max.z, 1.0}},
        {tmat * glm::vec4{bounds.min.x, bounds.min.y, bounds.min.z, 1.0}},

        {tmat * glm::vec4{bounds.max.x, bounds.min.y, bounds.min.z, 1.0}},
        {tmat * glm::vec4{bounds.min.x, bounds.max.y, bounds.min.z, 1.0}},
        {tmat * glm::vec4{bounds.min.x, bounds.min.y, bounds.max.z, 1.0}},

        {tmat * glm::vec4{bounds.max.x, bounds.min.y, bounds.max.z, 1.0}},
        {tmat * glm::vec4{bounds.min.x, bounds.max.y, bounds.max.z, 1.0}},
        {tmat * glm::vec4{bounds.max.x, bounds.max.y, bounds.min.z, 1.0}}};
    // find max & min
    glm::vec3 min = glm::vec3{FLT_MAX};
    glm::vec3 max = glm::vec3{-FLT_MAX};
    for (auto &corner : boundsCorners)
    {
        min.x = std::min(corner.x, min.x);
        min.y = std::min(corner.y, min.y);
        min.z = std::min(corner.z, min.z);

        max.x = std::max(corner.x, max.x);
        max.y = std::max(corner.y, max.y);
        max.z = std::max(corner.z, max.z);
    }

    return {min, max};
}

void Object::flushCaches()
{
    Cube::instance_counter = 0;
    FileObject::instance_counter = 0;
    //FileObject::path_cache.clear();
    UVSphere::instance_counter = 0;
    Plane::instance_counter = 0;
    //AssimpMeshObject::instance_counter = 0;
}