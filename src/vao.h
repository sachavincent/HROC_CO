#ifndef VAO_H
#define VAO_H

#include <glad/glad.h>
#include <vector>

class Vao
{
public:
    Vao(bool useEBO);

    void bind();

    void unbind();

    void del();

    inline void setVertexCount(int vertexCount) { this->_vertexCount = vertexCount; }

    inline int getVertexCount() { return this->_vertexCount; }

    inline GLuint& vboId() { return this->_vboId; }
    
    inline GLuint& eboId() { return this->_eboId; }

    inline const bool& useEBO() const { return this->_useEBO; }
private:
    void bindEBO();
    void unbindEBO();
private:
    GLuint _id;
    GLuint _vboId;
    GLuint _eboId;
    
    bool _useEBO;

    int _vertexCount;
};

#endif // VAO_H
