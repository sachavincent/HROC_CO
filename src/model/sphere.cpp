#include "object.hpp"

OBJECT_DATA UVSphere::data;
bool *UVSphere::visible;

unsigned int UVSphere::instance_counter = 0;

UVSphere::UVSphere(float _radius, int _nCols, int _nRows, const std::string &_name) : Object(_name)
{
    instance = instance_counter++;

    visible[instance] = true;
}

void UVSphere::create(int nbInstances)
{
    data.numInstances = nbInstances;
    visible = new bool[nbInstances];

    int _nCols = 25;
    int _nRows = 20;
    float _radius = 1.0f;
    float x, y, z, xy, s, t;

    data.bounds.min = {-_radius, -_radius, -_radius};
    data.bounds.max = {_radius, _radius, _radius};
    
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> normals;
    std::vector<GLfloat> textureCoord;
    std::vector<GLuint> indices;
    vertices.reserve(3 * _nCols * _nRows);
    normals.reserve(3 * _nCols * _nRows);
    indices.reserve(6 * _nCols * _nRows);
    textureCoord.reserve(2 * _nCols * _nRows);

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
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // add normals
            normals.push_back(x * r_inv);
            normals.push_back(y * r_inv);
            normals.push_back(z * r_inv);

            // add indices
            if (i != 0 && i <= (_nCols - 1))
            {
                indices.push_back(a);
                indices.push_back(b);
                indices.push_back(a + 1);
            }
            if (i <= (_nCols - 2))
            {
                indices.push_back(a + 1);
                indices.push_back(b);
                indices.push_back(b + 1);
            }

            s = (float)j / _nRows;
            t = (float)i / _nCols;
            textureCoord.push_back(s);
            textureCoord.push_back(t);
        }
    }

    data.numVertices = vertices.size() / 3;
    data.vertices = new Vertex[data.numVertices];
    for (size_t i = 0; i < data.numVertices; i++)
    {
        Vertex vertex;
        vertex.Position = {vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2]};
        vertex.Normal = {normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]};
        vertex.TexCoord = {textureCoord[i * 2], textureCoord[i * 2 + 1]};
        data.vertices[i] = vertex;
    }

    data.numIndices = indices.size();
    data.indices = new GLuint[data.numIndices];
    for (size_t i = 0; i < data.numIndices; i++)
        data.indices[i] = indices[i];
}

OBJECT_DATA UVSphere::getData()
{
    return data;
}