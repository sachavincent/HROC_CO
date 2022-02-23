#include "object.hpp"

unsigned int Plane::instance_counter = 0;

Plane::Plane(glm::vec2 _size, int _nX, int _nY, const std::string &_name,std::string _keyModel ) : Object(_name,_keyModel)
{
    instance = instance_counter++;
    setPosition(glm::vec3{0.0});
    setScale(glm::vec3(_size, 0));
    setRotationMatrix(glm::mat4{1.0});

    //visible[instance] = true;
}

// void Plane::create(int nbInstances)
// {
//     /*
//     data.numInstances = nbInstances;
//     visible = new bool[nbInstances];
//     data.bounds.min = {-0.5f, -0.5f, 0.0f};
//     data.bounds.max = {0.5f, 0.5f, 0.0f};

//     std::vector<GLfloat> vertices = {
//         -0.5f, 0.5f, 0.0f,
//         -0.5f, -0.5f, 0.0f,
//         0.5f, 0.5f, 0.0f,
//         0.5f, -0.5f, 0.0f};

//     std::vector<GLfloat> normals = {
//         0.0f, 0.0f, 1.0f,
//         0.0f, 0.0f, 1.0f,
//         0.0f, 0.0f, 1.0f,
//         0.0f, 0.0f, 1.0f};

//     std::vector<GLfloat> textureCoord = {
//         1.0f, 0.0f,
//         0.0f, 1.0f,
//         0.0f, 0.0f,
//         1.0f, 1.0f};

//     std::vector<GLuint> indices = {
//         1, 2, 0,
//         1, 3, 2};

//     data.numVertices = vertices.size() / 3;
//     data.vertices = new Vertex[data.numVertices];
//     for (size_t i = 0; i < data.numVertices; i++)
//     {
//         Vertex vertex;
//         vertex.Position = {vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2]};
//         vertex.Normal = {normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]};
//         vertex.TexCoord = {textureCoord[i * 2], textureCoord[i * 2 + 1]};
//         data.vertices[i] = vertex;
//     }

//     data.numIndices = indices.size();
//     data.indices = new GLuint[data.numIndices];
//     for (size_t i = 0; i < data.numIndices; i++)
//         data.indices[i] = indices[i];
//         */
// }
