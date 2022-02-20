#include "frustum.hpp"

bool FrustumObject::loaded = false;
GLuint FrustumObject::vao;
int FrustumObject::numIndices;

FrustumObject::FrustumObject(std::string _parentName, const glm::vec3 _edge[8],const glm::vec3 &_pos, const glm::mat4 &_rotationMatrix, const glm::vec3 &_scale) : Object("Frustum_" + _parentName)
{

	setPosition(_pos);
	setRotationMatrix(_rotationMatrix);
	setScale(_scale);
	std::cout << "Created frustum object: '" << name << "'" << std::endl;
	if (!loaded)
	{
        /*
            glm::vec4 frustumEdge[8] = {
            // near
            {-1, -1, -1, 1}, { 1, -1, -1, 1}, { 1,  1, -1, 1},  {-1,  1, -1, 1},
            // far
            {-1, -1, 1, 1},	{ 1, -1, 1, 1},	{ 1,  1, 1, 1},  {-1,  1, 1, 1}     

        };

        */
        std::vector<GLfloat> vertices ={
            _edge[0].x,_edge[0].y,_edge[0].z,
            _edge[4].x,_edge[4].y,_edge[4].z,
            
            _edge[1].x,_edge[1].y,_edge[1].z,
            _edge[5].x,_edge[5].y,_edge[5].z,
            
            _edge[2].x,_edge[2].y,_edge[2].z,
            _edge[6].x,_edge[6].y,_edge[6].z,
            
            _edge[3].x,_edge[3].y,_edge[3].z,
            _edge[7].x,_edge[7].y,_edge[7].z,
            
            _edge[0].x,_edge[0].y,_edge[0].z,
            _edge[1].x,_edge[1].y,_edge[1].z,
            
            _edge[0].x,_edge[0].y,_edge[0].z,
            _edge[3].x,_edge[3].y,_edge[3].z,
            
            _edge[2].x,_edge[2].y,_edge[2].z,
            _edge[1].x,_edge[1].y,_edge[1].z,
            
            _edge[3].x,_edge[3].y,_edge[3].z,
            _edge[2].x,_edge[2].y,_edge[2].z,
            
            _edge[4].x,_edge[4].y,_edge[4].z,
            _edge[5].x,_edge[5].y,_edge[5].z,
            
            _edge[4].x,_edge[4].y,_edge[4].z,
            _edge[7].x,_edge[7].y,_edge[7].z,
            
            _edge[6].x,_edge[6].y,_edge[6].z,
            _edge[5].x,_edge[5].y,_edge[5].z,
            
            _edge[7].x,_edge[7].y,_edge[7].z,
            _edge[6].x,_edge[6].y,_edge[6].z,
            
        };
		std::vector<GLuint> indices = {
            0,1,
            2,3,
            4,5,
            6,7,
            8,9,
            10,11,
            12,13,
            14,15,
            16,17,
            18,19,
            20,21,
            22,23
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
		loaded = true;
	}
}

void FrustumObject::draw(Shader &_shader)
{
	if (!loaded)
		return;
	_shader.loadMat4("model", getTransformationMatrix());
	glDrawElements(GL_LINES, numIndices, GL_UNSIGNED_INT, nullptr);
}

void FrustumObject::drawQuery(Shader &_shader)
{
	/*if (!loaded)
		return;

	glBindVertexArray(data.vao);

	_shader.loadMat4("model", position * rotation * scale);

	glDrawElements(GL_TRIANGLES, data.indices.size(), GL_UNSIGNED_INT, nullptr);

	glBindVertexArray(0);*/
}