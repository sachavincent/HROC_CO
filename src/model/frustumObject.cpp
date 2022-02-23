#include "frustum.hpp"

GLuint FrustumObject::vao;
FrustumObject::FrustumObject(std::string _parentName, const glm::vec3 _edge[8],const std::string _keyModel ) : Object("Frustum_" + _parentName,_keyModel)
{
	std::cout << "Created frustum object: '" << name << "'" << std::endl;
    
		vertices = {
			_edge[4].x,_edge[4].y,_edge[4].z,
			_edge[5].x,_edge[5].y,_edge[5].z,
			_edge[6].x,_edge[6].y,_edge[6].z,
			_edge[6].x,_edge[6].y,_edge[6].z,
			_edge[7].x,_edge[7].y,_edge[7].z,
			_edge[4].x,_edge[4].y,_edge[4].z,

			_edge[3].x,_edge[3].y,_edge[3].z,
			_edge[7].x,_edge[7].y,_edge[7].z,
			_edge[4].x,_edge[4].y,_edge[4].z,
			_edge[4].x,_edge[4].y,_edge[4].z,
			_edge[0].x,_edge[0].y,_edge[0].z,
			_edge[3].x,_edge[3].y,_edge[3].z,

			_edge[2].x,_edge[2].y,_edge[2].z,
			_edge[6].x,_edge[6].y,_edge[6].z,
			_edge[5].x,_edge[5].y,_edge[5].z,
			_edge[5].x,_edge[5].y,_edge[5].z,
			_edge[1].x,_edge[1].y,_edge[1].z,
			_edge[2].x,_edge[2].y,_edge[2].z,

			_edge[4].x,_edge[4].y,_edge[4].z,
			_edge[5].x,_edge[5].y,_edge[5].z,
			_edge[1].x,_edge[1].y,_edge[1].z,
			_edge[1].x,_edge[1].y,_edge[1].z,
			_edge[0].x,_edge[0].y,_edge[0].z,
			_edge[4].x,_edge[4].y,_edge[4].z,

			_edge[7].x,_edge[7].y,_edge[7].z,
			_edge[6].x,_edge[6].y,_edge[6].z,
			_edge[2].x,_edge[2].y,_edge[2].z,
			_edge[2].x,_edge[2].y,_edge[2].z,
			_edge[3].x,_edge[3].y,_edge[3].z,
			
			_edge[7].x,_edge[7].y,_edge[7].z,
            };

    outlineVertices = {
        _edge[0].x,
        _edge[0].y,
        _edge[0].z,
        _edge[4].x,
        _edge[4].y,
        _edge[4].z,

        _edge[1].x,
        _edge[1].y,
        _edge[1].z,
        _edge[5].x,
        _edge[5].y,
        _edge[5].z,

        _edge[2].x,
        _edge[2].y,
        _edge[2].z,
        _edge[6].x,
        _edge[6].y,
        _edge[6].z,

        _edge[3].x,
        _edge[3].y,
        _edge[3].z,
        _edge[7].x,
        _edge[7].y,
        _edge[7].z,

        _edge[0].x,
        _edge[0].y,
        _edge[0].z,
        _edge[1].x,
        _edge[1].y,
        _edge[1].z,

        _edge[0].x,
        _edge[0].y,
        _edge[0].z,
        _edge[3].x,
        _edge[3].y,
        _edge[3].z,

        _edge[2].x,
        _edge[2].y,
        _edge[2].z,
        _edge[1].x,
        _edge[1].y,
        _edge[1].z,

        _edge[3].x,
        _edge[3].y,
        _edge[3].z,
        _edge[2].x,
        _edge[2].y,
        _edge[2].z,

        _edge[4].x,
        _edge[4].y,
        _edge[4].z,
        _edge[5].x,
        _edge[5].y,
        _edge[5].z,

        _edge[4].x,
        _edge[4].y,
        _edge[4].z,
        _edge[7].x,
        _edge[7].y,
        _edge[7].z,

        _edge[6].x,
        _edge[6].y,
        _edge[6].z,
        _edge[5].x,
        _edge[5].y,
        _edge[5].z,

        _edge[7].x,
        _edge[7].y,
        _edge[7].z,
        _edge[6].x,
        _edge[6].y,
        _edge[6].z,
    };
		glGenBuffers(1, &vbo);
		glGenVertexArrays(1, &vao);

		// Bind the vao
		bind();
		// create and fill vertex data
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(),
					 GL_STREAM_DRAW);
		// set vertex attribute pointer
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
		glEnableVertexAttribArray(0);

		// Unbind the VAO
		unbind();
}
void FrustumObject::adjustVertexData(const glm::vec3 _edge[8])
{
		vertices = {
			_edge[4].x,_edge[4].y,_edge[4].z,
			_edge[5].x,_edge[5].y,_edge[5].z,
			_edge[6].x,_edge[6].y,_edge[6].z,
			_edge[6].x,_edge[6].y,_edge[6].z,
			_edge[7].x,_edge[7].y,_edge[7].z,
			_edge[4].x,_edge[4].y,_edge[4].z,

			_edge[3].x,_edge[3].y,_edge[3].z,
			_edge[7].x,_edge[7].y,_edge[7].z,
			_edge[4].x,_edge[4].y,_edge[4].z,
			_edge[4].x,_edge[4].y,_edge[4].z,
			_edge[0].x,_edge[0].y,_edge[0].z,
			_edge[3].x,_edge[3].y,_edge[3].z,

			_edge[2].x,_edge[2].y,_edge[2].z,
			_edge[6].x,_edge[6].y,_edge[6].z,
			_edge[5].x,_edge[5].y,_edge[5].z,
			_edge[5].x,_edge[5].y,_edge[5].z,
			_edge[1].x,_edge[1].y,_edge[1].z,
			_edge[2].x,_edge[2].y,_edge[2].z,

			_edge[4].x,_edge[4].y,_edge[4].z,
			_edge[5].x,_edge[5].y,_edge[5].z,
			_edge[1].x,_edge[1].y,_edge[1].z,
			_edge[1].x,_edge[1].y,_edge[1].z,
			_edge[0].x,_edge[0].y,_edge[0].z,
			_edge[4].x,_edge[4].y,_edge[4].z,

			_edge[7].x,_edge[7].y,_edge[7].z,
			_edge[6].x,_edge[6].y,_edge[6].z,
			_edge[2].x,_edge[2].y,_edge[2].z,
			_edge[2].x,_edge[2].y,_edge[2].z,
			_edge[3].x,_edge[3].y,_edge[3].z,
			
			_edge[7].x,_edge[7].y,_edge[7].z,
            };
    outlineVertices = {
        _edge[0].x,
        _edge[0].y,
        _edge[0].z,
        _edge[4].x,
        _edge[4].y,
        _edge[4].z,

        _edge[1].x,
        _edge[1].y,
        _edge[1].z,
        _edge[5].x,
        _edge[5].y,
        _edge[5].z,

        _edge[2].x,
        _edge[2].y,
        _edge[2].z,
        _edge[6].x,
        _edge[6].y,
        _edge[6].z,

        _edge[3].x,
        _edge[3].y,
        _edge[3].z,
        _edge[7].x,
        _edge[7].y,
        _edge[7].z,

        _edge[0].x,
        _edge[0].y,
        _edge[0].z,
        _edge[1].x,
        _edge[1].y,
        _edge[1].z,

        _edge[0].x,
        _edge[0].y,
        _edge[0].z,
        _edge[3].x,
        _edge[3].y,
        _edge[3].z,

        _edge[2].x,
        _edge[2].y,
        _edge[2].z,
        _edge[1].x,
        _edge[1].y,
        _edge[1].z,

        _edge[3].x,
        _edge[3].y,
        _edge[3].z,
        _edge[2].x,
        _edge[2].y,
        _edge[2].z,

        _edge[4].x,
        _edge[4].y,
        _edge[4].z,
        _edge[5].x,
        _edge[5].y,
        _edge[5].z,

        _edge[4].x,
        _edge[4].y,
        _edge[4].z,
        _edge[7].x,
        _edge[7].y,
        _edge[7].z,

        _edge[6].x,
        _edge[6].y,
        _edge[6].z,
        _edge[5].x,
        _edge[5].y,
        _edge[5].z,
        _edge[7].x,
        _edge[7].y,
        _edge[7].z,
        _edge[6].x,
        _edge[6].y,
        _edge[6].z,
    };
    
}
void FrustumObject::draw()
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER,0, vertices.size() * sizeof(GLfloat), vertices.data());
    glDrawArrays(GL_TRIANGLES, 0, 90);
}

void FrustumObject::drawOutline()
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER,0, outlineVertices.size() * sizeof(GLfloat), outlineVertices.data());
    glDrawArrays(GL_LINES, 0, 24);
}