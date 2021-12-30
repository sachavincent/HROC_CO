#include "vao.h"

Vao::Vao(bool useEBO): _useEBO(useEBO), _vertexCount(0), _eboId(0), _vboId(0)
{
    glGenVertexArrays(1, &this->_id);
    if (useEBO)
        glGenBuffers(1, &this->_eboId);

    glBindVertexArray(this->_id);
}

void Vao::bindEBO()
{
    if(this->_useEBO)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_eboId);
}

void Vao::unbindEBO()
{
    if(this->_useEBO)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Vao::bind()
{
    glBindVertexArray(this->_id);
    glBindBuffer(GL_ARRAY_BUFFER, this->_vboId);

    bindEBO();
}

void Vao::unbind()
{
    unbindEBO();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Vao::del()
{
    glDeleteVertexArrays(1, &this->_id);
}