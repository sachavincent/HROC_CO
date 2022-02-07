#include "boundingBoxObject.hpp"
#include "scene.hpp"

BoundingBoxObject::BoundingBoxObject(std::string _parentName, const glm::vec3 &_pos, const glm::mat4 &_rotationMatrix, const glm::vec3 &_scale)
	: Cube(1.0f, "BoundingBox_" + _parentName)
{
	setPosition(_pos);
	setRotationMatrix(_rotationMatrix);
	setScale(_scale);

	std::cout << "Created bounding box object: '" << name << "'" << std::endl;
}

void BoundingBoxObject::draw(Scene *_scene, int _depth)
{
	if (!loaded)
		return;

	//std::cout << "Rendering bounding box object " << name << " with depth = " << _depth << std::endl;
	_scene->getShader().loadInt("depthBB", _depth);
	Cube::draw(_scene);
}
