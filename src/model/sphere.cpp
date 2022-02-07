#include "object.hpp"

int UVSphere::instance = 0;
UVSphere::UVSphere(float _radius, int _nCols, int _nRows) : Object()
{
    name = "UVSphere_" + std::to_string(instance);
    instance++;

    float x, y, z, xy, s, t;

    m.vertices.reserve(3 * _nCols * _nRows);
    m.normals.reserve(3 * _nCols * _nRows);
    m.indices.reserve(6 * _nCols * _nRows);
    m.textureCoord.reserve(2 * _nCols * _nRows);

    float pi = 3.1415f;
    float r_inv = 1.0f / _radius; // for normals
    float row_step = 2 * pi / _nRows;
    float col_step = pi / _nCols;

    for (int i = 0; i <= _nCols; ++i)
    {
        xy = _radius * cosf(pi / 2 - i * col_step);
        z = _radius * sinf(pi / 2 - i * col_step);
        int a = i * (_nRows + 1);
        int b = a + _nRows + 1;

        for (int j = 0; j <= _nRows; ++j, ++a, ++b)
        {
            // add vertices
            x = xy * cosf(j * row_step);
            y = xy * sinf(j * row_step);
            m.vertices.push_back(x);
            m.vertices.push_back(y);
            m.vertices.push_back(z);

            // add normals
            m.normals.push_back(x * r_inv);
            m.normals.push_back(y * r_inv);
            m.normals.push_back(z * r_inv);

            // add indices
            if (i != 0 && i <= (_nCols - 1))
            {
                pushIndices(a, b, a + 1);
            }
            if (i <= (_nCols - 2))
            {
                pushIndices(a + 1, b, b + 1);
            }

            s = (float)j / _nRows;
            t = (float)i / _nCols;
            m.textureCoord.push_back(s);
            m.textureCoord.push_back(t);
        }
    }

    position = glm::vec3{1.0};
    scale = glm::vec3{1.0};
    rotationMatrix = glm::mat4{1.0};
}

void inline UVSphere::pushIndices(int ind_1, int ind_2, int ind_3)
{
    m.indices.push_back(ind_1);
    m.indices.push_back(ind_2);
    m.indices.push_back(ind_3);
}