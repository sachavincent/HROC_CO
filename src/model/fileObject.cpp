#include "object.hpp"
#include "scene.hpp"
#include "utils/utils.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

std::map<const std::string, bool *> FileObject::visible;

unsigned int FileObject::instance_counter = 0;

std::map<const std::string, OBJECT_DATA> FileObject::path_cache;

FileObject::FileObject(const std::string &_path) : Object(_path.substr(_path.find_last_of("/") + 1) + "_" + std::to_string(instance_counter))
{
    instance = instance_counter++;

    if (path_cache.find(_path) == path_cache.end())
        std::cerr << "Unknown file path: '" << _path << "'!" << std::endl;
}

OBJECT_DATA FileObject::processMesh(aiMesh *_mesh, const aiScene *_scene)
{
    OBJECT_DATA data;

    std::vector<GLfloat> vertices;
    std::vector<GLfloat> normals;
    std::vector<GLfloat> textureCoord;
    std::vector<GLuint> indices;

    vertices.reserve(_mesh->mNumVertices * 3);
    normals.reserve(_mesh->mNumVertices * 3);
    indices.reserve(_mesh->mNumFaces * 3);
    // add vertices
    for (size_t i = 0; i < _mesh->mNumVertices; i++)
    {
        vertices.insert(vertices.end(), {_mesh->mVertices[i].x, _mesh->mVertices[i].y, _mesh->mVertices[i].z});
    }
    // add normals
    for (size_t i = 0; i < _mesh->mNumVertices; i++)
    {
        normals.insert(normals.end(), {_mesh->mNormals[i].x, _mesh->mNormals[i].y, _mesh->mNormals[i].z});
    }
    // add indices
    for (size_t i = 0; i < _mesh->mNumFaces; i++)
    {
        aiFace face = _mesh->mFaces[i];
        for (size_t j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // add texture coordinates
    if (_mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
    {
        for (size_t i = 0; i < _mesh->mNumVertices; i++)
            textureCoord.insert(textureCoord.end(), {_mesh->mTextureCoords[0][i].x, _mesh->mTextureCoords[0][i].y});
    }

    // center object so that centroid is at (0,0,0)
    // this important for scaling/rotation etc..
    double meanX = 0, meanY = 0, meanZ = 0;

    for (size_t i = 0; i < vertices.size(); i += 3)
    {
        meanX += vertices[i];
        meanY += vertices[i + 1];
        meanZ += vertices[i + 2];
    }

    size_t ptsNum = vertices.size() / 3;
    meanX /= ptsNum;
    meanY /= ptsNum;
    meanZ /= ptsNum;

    data.bounds.max = glm::vec3{-FLT_MAX};
    data.bounds.min = glm::vec3{FLT_MAX};

    for (size_t i = 0; i < vertices.size(); i += 3)
    {
        vertices[i] -= (GLfloat)meanX;
        vertices[i + 1] -= (GLfloat)meanY;
        vertices[i + 2] -= (GLfloat)meanZ;

        data.bounds.max.x = std::max(data.bounds.max.x, vertices[i]);
        data.bounds.max.y = std::max(data.bounds.max.y, vertices[i + 1]);
        data.bounds.max.z = std::max(data.bounds.max.z, vertices[i + 2]);

        data.bounds.min.x = std::min(data.bounds.min.x, vertices[i]);
        data.bounds.min.y = std::min(data.bounds.min.y, vertices[i + 1]);
        data.bounds.min.z = std::min(data.bounds.min.z, vertices[i + 2]);
    }

    data.numVertices = vertices.size() / 3;
    data.vertices = new Vertex[data.numVertices];
    for (size_t i = 0; i < data.numVertices; i++)
    {
        Vertex vertex;
        vertex.Position = {vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2]};
        vertex.Normal = {normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]};
        if (_mesh->mTextureCoords[0])
            vertex.TexCoord = {textureCoord[i * 2], textureCoord[i * 2 + 1]};
        else
            vertex.TexCoord = {0, 0};

        data.vertices[i] = vertex;
    }

    data.numIndices = indices.size();
    data.indices = new GLuint[data.numIndices];
    for (size_t i = 0; i < data.numIndices; i++)
        data.indices[i] = indices[i];

    return data;
}

void FileObject::create(const std::string &_path, int nbInstances)
{
    bool _smoothNormals = true;

    std::string abs_path = Utils::workingDirectory() + _path;

    // skip loading if same object already exists
    if (path_cache.find(_path) == path_cache.end() || path_cache.size() == 0)
    {
        Assimp::Importer importer;

        std::cout << "loading object from file : " << abs_path << " ..." << std::endl;

        const aiScene *scene =
            _smoothNormals ? importer.ReadFile(abs_path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals)
                           : importer.ReadFile(abs_path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return;
        }
        if (scene->mNumMeshes != 1)
        {
            std::cerr << "File contains too many meshes" << std::endl;
            return;
        }

        aiMesh *mesh = scene->mMeshes[0];
        OBJECT_DATA data = processMesh(mesh, scene);
        data.numInstances = nbInstances;
        path_cache[_path] = data;
        visible[_path] = new bool[nbInstances];
    }
}

std::vector<OBJECT_DATA> FileObject::getData()
{
    std::vector<OBJECT_DATA> data;
    for (auto &pair : path_cache)
        data.push_back(pair.second);

    return data;
}