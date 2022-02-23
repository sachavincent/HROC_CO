#ifndef BBOBJECT_H
#define BBOBJECT_H

#include "object.hpp"

class BoundingBoxObject : public Object
{
private:
    static bool loaded;
    static Shader shader;
    static GLuint vao, vaoFull;
    static int numIndices, numIndicesFull;
    static OBJECT_BOUNDS bounds;

    glm::mat4 transformationMatrix;

public:
    BoundingBoxObject(std::string _parentName,
                      const glm::vec3 &_pos,
                      const glm::mat4 &_rotationMatrix,
                      const glm::vec3 &_scale,
                      std::string keyModel = "boundingbox");

    void draw(Shader &_shader, int _num);

    static void preDrawQuery();

    void drawQuery(Shader &_shader);

    static inline void bind() { glBindVertexArray(vao); }
    static inline void unbind() { glBindVertexArray(0); }

    inline OBJECT_BOUNDS getObjectBounds() const { return bounds; }
};

#endif
