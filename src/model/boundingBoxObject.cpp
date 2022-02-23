#include "bvh/boundingBoxObject.hpp"
#include "scene.hpp"

bool BoundingBoxObject::loaded = false;
GLuint BoundingBoxObject::vao;
int BoundingBoxObject::numIndices;
GLuint BoundingBoxObject::vaoFull;
int BoundingBoxObject::numIndicesFull;
OBJECT_BOUNDS BoundingBoxObject::bounds;

BoundingBoxObject::BoundingBoxObject(std::string _parentName, const glm::vec3 &_pos, const glm::mat4 &_rotationMatrix, const glm::vec3 &_scale, std::string keyModel) : Object("BoundingBox_" + _parentName, keyModel)
{
	setPosition(_pos);
	setRotationMatrix(_rotationMatrix);
	setScale(_scale);

	// std::cout << "Created bounding box object: '" << name << "'" << std::endl;
	if (!loaded)
	{
#ifndef HROC_TESTS
		bounds.min = {-0.5f, -0.5f, -0.5f};
		bounds.max = {0.5f, 0.5f, 0.5f};
		std::vector<GLfloat> vertices = {
			-0.5f,
			-0.5f,
			0.5f,
			-0.5f,
			-0.5f,
			-0.5f,

			0.5f,
			-0.5f,
			0.5f,
			0.5f,
			-0.5f,
			-0.5f,

			0.5f,
			0.5f,
			0.5f,
			0.5f,
			0.5f,
			-0.5f,

			-0.5f,
			0.5f,
			0.5f,
			-0.5f,
			0.5f,
			-0.5f,

			-0.5f,
			-0.5f,
			0.5f,
			0.5f,
			-0.5f,
			0.5f,

			-0.5f,
			-0.5f,
			0.5f,
			-0.5f,
			0.5f,
			0.5f,

			0.5f,
			0.5f,
			0.5f,
			0.5f,
			-0.5f,
			0.5f,

			-0.5f,
			0.5f,
			0.5f,
			0.5f,
			0.5f,
			0.5f,

			-0.5f,
			-0.5f,
			-0.5f,
			0.5f,
			-0.5f,
			-0.5f,

			-0.5f,
			-0.5f,
			-0.5f,
			-0.5f,
			0.5f,
			-0.5f,

			0.5f,
			0.5f,
			-0.5f,
			0.5f,
			-0.5f,
			-0.5f,

			-0.5f,
			0.5f,
			-0.5f,
			0.5f,
			0.5f,
			-0.5f,
		};

		std::vector<GLuint> indices = {
			0, 1,
			2, 3,
			4, 5,
			6, 7,
			8, 9,
			10, 11,
			12, 13,
			14, 15,
			16, 17,
			18, 19,
			20, 21,
			22, 23

		};

		numIndices = indices.size();

		GLuint vbo, ebo;
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);
		glGenVertexArrays(1, &vao);

		// Bind the vao
		bind();

		// create and fill vertex data
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(),
					 GL_STATIC_DRAW);
		// set vertex attribute pointer
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
		glEnableVertexAttribArray(0);

		// copy indices to ebo
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLfloat), indices.data(), GL_STATIC_DRAW);

		// Unbind the VAO
		unbind();

		std::vector<GLfloat> verticesFull = {
			-0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, 0.5f, -0.5f,
			0.5f, 0.5f, -0.5f,
			-0.5f, 0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f, -0.5f, 0.5f,
			0.5f, -0.5f, 0.5f,
			0.5f, 0.5f, 0.5f,
			0.5f, 0.5f, 0.5f,
			-0.5f, 0.5f, 0.5f,
			-0.5f, -0.5f, 0.5f,

			-0.5f, 0.5f, 0.5f,
			-0.5f, 0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, 0.5f,
			-0.5f, 0.5f, 0.5f,

			0.5f, 0.5f, 0.5f,
			0.5f, 0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, 0.5f,
			0.5f, 0.5f, 0.5f,

			-0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, 0.5f,
			0.5f, -0.5f, 0.5f,
			-0.5f, -0.5f, 0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f, 0.5f, -0.5f,
			0.5f, 0.5f, -0.5f,
			0.5f, 0.5f, 0.5f,
			0.5f, 0.5f, 0.5f,
			-0.5f, 0.5f, 0.5f,
			-0.5f, 0.5f, -0.5f};

		std::vector<GLuint> indicesFull = {
			0, 2, 1,
			3, 5, 4,

			6, 7, 8,
			9, 10, 11,

			12, 13, 14,
			15, 16, 17,

			18, 20, 19,
			21, 23, 22,

			24, 25, 26,
			27, 28, 29,

			30, 32, 31,
			33, 35, 34};

		numIndicesFull = indicesFull.size();

		GLuint vboFull, eboFull;
		glGenBuffers(1, &vboFull);
		glGenBuffers(1, &eboFull);
		glGenVertexArrays(1, &vaoFull);

		// Bind the vao
		glBindVertexArray(vaoFull);

		// create and fill vertex data
		glBindBuffer(GL_ARRAY_BUFFER, vboFull);
		glBufferData(GL_ARRAY_BUFFER, verticesFull.size() * sizeof(GLfloat), verticesFull.data(), GL_STATIC_DRAW);
		// set vertex attribute pointer
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
		glEnableVertexAttribArray(0);

		// copy indices to ebo
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboFull);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesFull.size() * sizeof(GLfloat), indicesFull.data(), GL_STATIC_DRAW);

		// Unbind the VAO
		unbind();

#endif
		loaded = true;
	}
}

void BoundingBoxObject::draw(Shader &_shader, int _numBB)
{
	if (!loaded)
		return;

	_shader.loadInt("numBB", _numBB);
	_shader.loadMat4("model", getTransformationMatrix());

	glDrawElements(GL_LINES, numIndices, GL_UNSIGNED_INT, nullptr);
}

void BoundingBoxObject::preDrawQuery()
{
	if (!loaded)
		return;
		
	glBindVertexArray(vaoFull);
}

void BoundingBoxObject::drawQuery(Shader &_shader)
{
	if (!loaded)
		return;
		
	_shader.loadMat4("model", getTransformationMatrix());

	glDrawElements(GL_TRIANGLES, numIndicesFull, GL_UNSIGNED_INT, nullptr);
}