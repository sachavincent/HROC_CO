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

void BoundingBoxObject::draw(Shader &_shader, int _num)
{
	if (!loaded)
		return;

	_shader.loadInt("numBB", _num);
	Cube::draw(_shader);
}

void BoundingBoxObject::drawQuery(Shader &_shader)
{
	if (!loaded)
		return;

	glBindVertexArray(m.vao);

	_shader.loadMat4("model", position * rotation * scale);

	glDrawElements(GL_TRIANGLES, m.indices.size(), GL_UNSIGNED_INT, nullptr);

	glBindVertexArray(0);
}