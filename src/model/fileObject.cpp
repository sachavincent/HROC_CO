#include "object.hpp"
#include "scene.hpp"
#include "utils/utils.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

int FileObject::instance_counter = 0;

std::map<const std::string, std::tuple<uint32_t, uint32_t, Object::OBJECT_BOUNDS>>
    FileObject::path_cache;

FileObject::FileObject(std::string _path, bool _smoothNormals, std::string _name)
{
    instance = instance_counter;
    instance_counter++;

    name = _path.substr(_path.find_last_of("/") + 1) + "_" + std::to_string(instance);

    abs_path = Utils::workingDirectory() + _path;

    // skip loading if same object already exists
    if (path_cache.find(abs_path) == path_cache.end() || path_cache.size() == 0)
    {

        Assimp::Importer importer;

        std::cout << "loading object from file : " << abs_path << " ..." << std::endl;

        const aiScene *scene =
            _smoothNormals ? importer.ReadFile(abs_path, aiProcess_Triangulate | aiProcess_FlipUVs |
                                                             aiProcess_GenSmoothNormals)
                           : importer.ReadFile(abs_path, aiProcess_Triangulate | aiProcess_FlipUVs |
                                                             aiProcess_GenNormals);

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
        processMesh(mesh, scene);
        path_cache[abs_path] = {-1, m.numIndices, bounds};
    }
}

void FileObject::processMesh(aiMesh *_mesh, const aiScene *_scene)
{

    m.vertices.reserve(_mesh->mNumVertices * 3);
    m.normals.reserve(_mesh->mNumVertices * 3);
    m.indices.reserve(_mesh->mNumFaces * 3);
    // add vertices
    for (size_t i = 0; i < _mesh->mNumVertices; i++)
    {
        m.vertices.insert(m.vertices.end(),
                          {_mesh->mVertices[i].x, _mesh->mVertices[i].y, _mesh->mVertices[i].z});
    }
    // add normals
    for (size_t i = 0; i < _mesh->mNumVertices; i++)
    {
        m.normals.insert(m.normals.end(),
                         {_mesh->mNormals[i].x, _mesh->mNormals[i].y, _mesh->mNormals[i].z});
    }
    // add indices
    for (size_t i = 0; i < _mesh->mNumFaces; i++)
    {
        aiFace face = _mesh->mFaces[i];
        for (size_t j = 0; j < face.mNumIndices; j++)
            m.indices.push_back(face.mIndices[j]);
    }
    m.numIndices = m.indices.size();

    // add texture coordinates
    if (_mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
    {
        for (size_t i = 0; i < _mesh->mNumVertices; i++)
            m.textureCoord.insert(m.textureCoord.end(),
                                  {_mesh->mTextureCoords[0][i].x, _mesh->mTextureCoords[0][i].y});
    }

    // center object so that centroid is at (0,0,0)
    // this important for scaling/rotation etc..
    double meanX = 0, meanY = 0, meanZ = 0;

    for (size_t i = 0; i < m.vertices.size(); i += 3)
    {
        meanX += m.vertices[i];
        meanY += m.vertices[i + 1];
        meanZ += m.vertices[i + 2];
    }

    size_t ptsNum = m.vertices.size() / 3;
    meanX /= ptsNum;
    meanY /= ptsNum;
    meanZ /= ptsNum;

    bounds.max = glm::vec3{-FLT_MAX};
    bounds.min = glm::vec3{FLT_MAX};

    for (size_t i = 0; i < m.vertices.size(); i += 3)
    {

        m.vertices[i] -= (GLfloat)meanX;
        m.vertices[i + 1] -= (GLfloat)meanY;
        m.vertices[i + 2] -= (GLfloat)meanZ;

        bounds.max.x = std::max(bounds.max.x, m.vertices[i]);
        bounds.max.y = std::max(bounds.max.y, m.vertices[i + 1]);
        bounds.max.z = std::max(bounds.max.z, m.vertices[i + 2]);

        bounds.min.x = std::min(bounds.min.x, m.vertices[i]);
        bounds.min.y = std::min(bounds.min.y, m.vertices[i + 1]);
        bounds.min.z = std::min(bounds.min.z, m.vertices[i + 2]);
    }
}

void FileObject::load()
{

    // trouver la pair, si first = -1 -> skip
    auto &it = path_cache.find(abs_path)->second;

    if (std::get<0>(it) == -1)
    { // only load first object data (vao of .obj not set)
        Object::load();
        std::get<0>(it) = m.vao; // set vao
    }
    else
    { // copy vao and bounds from original object.
        m.vao = std::get<0>(it);
        m.numIndices = std::get<1>(it);
        bounds = std::get<2>(it);
        loaded = true;
        Object::load();
    }
}