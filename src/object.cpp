#include "object.hpp"
#include "boundingbox.hpp"

Object::Object(glm::vec3 pos, int vao, int vbo, int ebo) : _vao(vao), _vbo(vbo), _ebo(ebo)
{
}
Object::Object(glm::vec3 pos, const std::string &file)
{
}

Object::~Object()
{
    delete (_boundingBox);
    delete (_observer);
}

void Object::registerObserver(Observer &o)
{
    _observer = &o;
}

void Object::removeObserver(Observer &o)
{
    _observer = nullptr;
}

void Object::notifyObservers()
{
    _observer->update(this, _visible);
}

void Object::draw()
{
}