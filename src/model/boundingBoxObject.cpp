#include "bvh/boundingBoxObject.hpp"
#include "scene.hpp"

BoundingBoxObject::BoundingBoxObject(std::string _parentName, const glm::vec3 &_pos, const glm::mat4 &_rotationMatrix, const glm::vec3 &_scale)
	: Cube(1.0f, "BoundingBox_" + _parentName)
{
	setPosition(_pos);
	setRotationMatrix(_rotationMatrix);
	setScale(_scale);

	std::cout << "Created bounding box object: '" << name << "'" << std::endl;
}

void BoundingBoxObject::draw(Scene *_scene, int _num)
{
	if (!loaded)
		return;

	_scene->getShader().loadInt("numBB", _num);
	Cube::draw(_scene);
}

void BoundingBoxObject::drawQuery(Scene *_scene)
{
	if (!loaded)
		return;

	glBindVertexArray(m.vao);

	Shader &sh = _scene->getSimpleShader();
	sh.loadMat4("model", transformationMatrix);
	
	glDrawElements(GL_TRIANGLES, m.indices.size(), GL_UNSIGNED_INT, nullptr);

	glBindVertexArray(0);
}