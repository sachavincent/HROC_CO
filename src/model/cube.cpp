#include "object.hpp"
#include "scene.hpp"
#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"
#include <algorithm>

//OBJECT_DATA Cube::data;
//bool *Cube::visible;

unsigned int Cube::instance_counter = 0;

Cube::Cube(float _edgeSize, const std::string &_name,std::string _keyModel) 
	: Object(_name,_keyModel)
{
	instance = instance_counter++;
	setPosition(glm::vec3{0.0});
	setScale(glm::vec3(_edgeSize));
	setRotationMatrix(glm::mat4{1.0});
}
