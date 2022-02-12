#include "object.hpp"
#include "scene.hpp"
#include "utils.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

int FileObject::instance = 0;

FileObject::FileObject(std::string _path, bool _smoothNormals) : FileObject(_path, _smoothNormals, _path.substr(_path.find_last_of("/") + 1) + "_" + std::to_string(instance))
{
}
FileObject::FileObject(std::string _path, bool _smoothNormals, std::string _name) : Object(_name)
{
	instance++;

	Assimp::Importer importer;

	_path = Utils::workingDirectory() + _path;

	std::cout << "loading object from file : "<< _path << " ..." << std::endl;

	const aiScene *scene = _smoothNormals ? importer.ReadFile(_path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals)
										  : importer.ReadFile(_path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

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

	// std::string directory = _path.substr(0, _path.find_last_of('/'));

	// configure each mesh
	for (size_t i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[i];
		position = glm::vec3{0.0};
		scale = glm::vec3{1.0};
		rotationMatrix = glm::mat4{1.0};
		processMesh(mesh, scene, i);
	}
	/*
		// center objects vertices (for scaling later)
		double meanX = 0, meanY = 0, meanZ = 0;
		size_t count = 0;
		for (auto &subObject : subObjects)
		{
			for (size_t i = 0; i < subObject.vertices.size(); i += 3)
			{
				count++;
				meanX += subObject.vertices[i];
				meanY += subObject.vertices[i + 1];
				meanZ += subObject.vertices[i + 2];
			}
		}

		meanX /= count;
		meanY /= count;
		meanZ /= count;
		for (auto &subObject : subObjects)
		{
			for (size_t i = 0; i < subObject.vertices.size(); i += 3)
			{
				count++;
				subObject.vertices[i] -= (GLfloat)meanX;
				subObject.vertices[i + 1] -= (GLfloat)meanY;
				subObject.vertices[i + 2] -= (GLfloat)meanZ;
			}
		}*/
}

void FileObject::processMesh(aiMesh *_mesh, const aiScene *_scene, size_t _meshIdx)
{
	// auto& tex = subObjects[_meshIdx].textureCoord;

	// add vertices
	for (size_t i = 0; i < _mesh->mNumVertices; i++)
	{
		m.vertices.insert(m.vertices.end(), {_mesh->mVertices[i].x, _mesh->mVertices[i].y, _mesh->mVertices[i].z});
	}
	// add normals
	for (size_t i = 0; i < _mesh->mNumVertices; i++)
	{
		m.normals.insert(m.normals.end(), {_mesh->mNormals[i].x, _mesh->mNormals[i].y, _mesh->mNormals[i].z});
	}
	// add indices
	for (size_t i = 0; i < _mesh->mNumFaces; i++)
	{
		aiFace face = _mesh->mFaces[i];
		for (size_t j = 0; j < face.mNumIndices; j++)
			m.indices.push_back(face.mIndices[j]);
	}
}