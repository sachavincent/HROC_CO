/*#include "object.hpp"

int AssimpMeshObject::instance_counter = 0;

AssimpMeshObject::AssimpMeshObject(const aiScene *_scene, aiMesh *_mesh, std::string _name)
{
    instance = instance_counter;
    instance_counter++;
    name = _name;

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

    if (_mesh->mMaterialIndex >= 0)
    {   
        std::string modelspath = "models\\";
        aiMaterial *material = _scene->mMaterials[_mesh->mMaterialIndex];
        if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
            aiString str;
            material->GetTexture(aiTextureType_DIFFUSE, 0, &str);
            diffuseMapPath = modelspath+ str.C_Str();
        }
        if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
            aiString str;
            material->GetTexture(aiTextureType_SPECULAR, 0, &str);
            specularMapPath = modelspath + str.C_Str();
        }
        aiColor4D color;
        if(AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &color)){
            diffuseColor = glm::vec3{color[0],color[1],color[2]};
        }
        if(AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &color)){
            specularColor = glm::vec3{color[0],color[1],color[2]}; 
        }

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

    position = glm::translate(glm::mat4{1.0},{meanX,meanY,meanZ});

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
*/