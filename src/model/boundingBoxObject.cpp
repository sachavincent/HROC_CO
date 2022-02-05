#include "object.hpp"
#include "scene.hpp"

BoundingBoxObject::BoundingBoxObject() : Cube()
{

}

BoundingBoxObject::BoundingBoxObject(float _edgeSize) : Cube(_edgeSize)
{
}

void BoundingBoxObject::draw(Scene *_scene, int depth)
{
	_scene->getShader().loadInt("depthBB", depth);
	std::cout << depth << std::endl;
	std::cout << translate[1][1] << std::endl;
	Cube::draw(_scene);
}
