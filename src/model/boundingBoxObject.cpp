#include "object.hpp"
#include "scene.hpp"

BoundingBoxObject::BoundingBoxObject(std::string _parentName, glm::vec3 center, glm::vec3 size, float _edgeSize)
	: Cube(_edgeSize, "BoundingBox_" + _parentName)
{
	setPosition(center);
	setScale(size);
	pos = center;
	std::cout << "Created bounding box '" << name << "'" << std::endl;
}

void BoundingBoxObject::draw(Scene *_scene, int _depth)
{
	std::cout << "Rendering bounding box object " << name << " at " << glm::to_string(pos) << " with depth = " << _depth << std::endl;
	_scene->getShader().loadInt("depthBB", _depth);
	Cube::draw(_scene);
}
