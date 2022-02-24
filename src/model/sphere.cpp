#include "object.hpp"

unsigned int UVSphere::instance_counter = 0;

UVSphere::UVSphere(const std::string &_name, std::string _modelkey) : Object(_name, _modelkey)
{
    instance = instance_counter++;
}