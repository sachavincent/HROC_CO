#include "object.hpp"
#include "scene.hpp"
#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"
#include <algorithm>

unsigned int Cube::instance_counter = 0;

Cube::Cube(const std::string &_name, std::string _keyModel) : Object(_name, _keyModel)
{
	instance = instance_counter++;

	setPosition(glm::vec3{0.0});
	setScale(glm::vec3(1));
	setRotationMatrix(glm::mat4{1.0});
}
