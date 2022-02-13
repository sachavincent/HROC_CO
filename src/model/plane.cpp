#include "object.hpp"

int Plane::instance = 0;

Plane::Plane(glm::vec2 _size, int _nX, int _nY, const std::string &_name) : Object(_name)
{
    instance++;

    GLuint idx = 0;

    float stepX = _size[0] / _nX;
    float stepY = _size[1] / _nY;

    for (int i = 0; i < _nX; i++)
    {
        for (int j = 0; j < _nY; j++)
        {

            m.vertices.insert(m.vertices.end(), {
                                                    -_size[0] / 2 + i * (stepX),
                                                    -_size[1] / 2 + j * (stepY),
                                                    0.0f,
                                                    -_size[0] / 2 + (i + 1) * (stepX),
                                                    -_size[1] / 2 + j * (stepY),
                                                    0.0f,
                                                    -_size[0] / 2 + (i + 1) * (stepX),
                                                    -_size[1] / 2 + (j + 1) * (stepY),
                                                    0.0f,

                                                    -_size[0] / 2 + (i + 1) * (stepX),
                                                    -_size[1] / 2 + (j + 1) * (stepY),
                                                    0.0f,
                                                    -_size[0] / 2 + i * (stepX),
                                                    -_size[1] / 2 + (j + 1) * (stepY),
                                                    0.0f,
                                                    -_size[0] / 2 + i * (stepX),
                                                    -_size[1] / 2 + j * (stepY),
                                                    0.0f,
                                                });
            m.normals.insert(m.normals.end(), {0.0f, 0.0f, 1.0f,
                                               0.0f, 0.0f, 1.0f,
                                               0.0f, 0.0f, 1.0f,

                                               0.0f, 0.0f, 1.0f,
                                               0.0f, 0.0f, 1.0f,
                                               0.0f, 0.0f, 1.0f});

            m.textureCoord.insert(m.textureCoord.end(), {
                                                            i * (stepX) / _size[0],
                                                            j * (stepY) / _size[1],
                                                            (i + 1) * (stepX) / _size[0],
                                                            j * (stepY) / _size[1],
                                                            (i + 1) * (stepX) / _size[0],
                                                            (j + 1) * (stepY) / _size[1],

                                                            (i + 1) * (stepX) / _size[0],
                                                            (j + 1) * (stepY) / _size[1],
                                                            i * (stepX) / _size[0],
                                                            (j + 1) * (stepY) / _size[1],
                                                            i * (stepX) / _size[0],
                                                            j * (stepY) / _size[1],
                                                        });

            m.indices.insert(m.indices.end(), {idx, idx + 2, idx + 1,
                                               idx + 3, idx + 5, idx + 4});
            idx += 6;
        }
    }
    position = glm::vec3{0.0};
    scale = glm::vec3{1.0};
    rotationMatrix = glm::mat4{1.0};
    bounds.min = {-_size[0] / 2,-_size[1] / 2, 0.0f};
	bounds.max = {_size[0] / 2,_size[1] / 2, 0.0f};
    m.numIndices = m.indices.size();
}