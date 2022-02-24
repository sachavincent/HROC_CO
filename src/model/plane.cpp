#include "object.hpp"

unsigned int Plane::instance_counter = 0;

Plane::Plane(const std::string &_name, std::string _keyModel) : Object(_name, _keyModel)
{
    instance = instance_counter++;
}