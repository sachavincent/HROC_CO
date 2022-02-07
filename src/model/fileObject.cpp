#include "object.hpp"
#include "scene.hpp"
#include "utils.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

int FileObject::instance = 0;

FileObject::FileObject(std::string _path, SMOOTH_NORMAL _smoothNormals) : Object()
{
	name = _path.substr(_path.find_last_of("/") + 1) + "_" + std::to_string(instance);
	instance++;

	Assimp::Importer importer;

	_path = Utils::workingDirectory() + _path;

	std::cout << "loading object from file : "
			  << _path << " ..." << std::endl;

	const aiScene *scene = _smoothNormals ? importer.ReadFile(_path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals)
										  : importer.ReadFile(_path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	// std::string directory = _path.substr(0, _path.find_last_of('/'));

	subObjects.resize(scene->mNumMeshes);
	// process each mesh of the object

	std::cout << "processing " << subObjects.size() << " submeshes ..." << std::endl;

	// configure each mesh
	for (size_t i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[i];
		position = glm::vec3{0.0};
		scale = glm::vec3{1.0};
		rotationMatrix = glm::mat4{1.0};
		processMesh(mesh, scene, i);
	}

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
	}
}

void FileObject::processMesh(aiMesh *_mesh, const aiScene *_scene, size_t _meshIdx)
{
	auto &vert = subObjects[_meshIdx].vertices;
	auto &ind = subObjects[_meshIdx].indices;
	auto &norm = subObjects[_meshIdx].normals;
	// auto& tex = subObjects[_meshIdx].textureCoord;

	// add vertices
	for (size_t i = 0; i < _mesh->mNumVertices; i++)
	{
		vert.insert(vert.end(),
					{_mesh->mVertices[i].x, _mesh->mVertices[i].y, _mesh->mVertices[i].z});
	}
	// add normals
	for (size_t i = 0; i < _mesh->mNumVertices; i++)
	{
		norm.insert(norm.end(),
					{_mesh->mNormals[i].x, _mesh->mNormals[i].y, _mesh->mNormals[i].z});
	}
	// add indices
	for (size_t i = 0; i < _mesh->mNumFaces; i++)
	{
		aiFace face = _mesh->mFaces[i];
		for (size_t j = 0; j < face.mNumIndices; j++)
			ind.push_back(face.mIndices[j]);
	}
}

//! Load vertex buffers and shader of cube
void FileObject::load()
{
	for (auto &subObject : subObjects)
	{

		// gen geometry buffers
		glGenBuffers(1, &subObject.vbo);
		glGenBuffers(1, &subObject.nbo);
		glGenBuffers(1, &subObject.tbo);
		glGenBuffers(1, &subObject.ebo);
		glGenVertexArrays(1, &subObject.vao);

		// Bind the vao
		glBindVertexArray(subObject.vao);

		// copy indices to vbo
		glBindBuffer(GL_ARRAY_BUFFER, subObject.vbo);
		glBufferData(GL_ARRAY_BUFFER, subObject.vertices.size() * sizeof(GLfloat), subObject.vertices.data(), GL_STATIC_DRAW);
		// define array for vertices
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
		glEnableVertexAttribArray(0);

		// copy normals to nbo
		glBindBuffer(GL_ARRAY_BUFFER, subObject.nbo);
		glBufferData(GL_ARRAY_BUFFER, subObject.normals.size() * sizeof(GLfloat), subObject.normals.data(), GL_STATIC_DRAW);
		// define array for normals
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
		glEnableVertexAttribArray(1);

		//// Copy texture coordinates array in element buffer
		// glBindBuffer(GL_ARRAY_BUFFER, subObject.tbo);
		// glBufferData(GL_ARRAY_BUFFER, subObject.textureCoord.size() * sizeof(GLfloat), subObject.textureCoord.data(), GL_STATIC_DRAW);
		//// define array for texture coordinates
		// glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
		// glEnableVertexAttribArray(2);

		// copy indices to ebo
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subObject.ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, subObject.indices.size() * sizeof(GLfloat), subObject.indices.data(), GL_STATIC_DRAW);

		// Unbind the VAO
		glBindVertexArray(0);

		// load right shader

		if ((diffuseMapPath != ""))
		{
			diffuseMap = Texture::loadTexture(diffuseMapPath.c_str());
		}
		if (specularMapPath != "")
		{
			specularMap = Texture::loadTexture(specularMapPath.c_str());
		}
	}
}

void FileObject::draw(Scene *_scene)
{
	Shader &sh = _scene->getShader();
	sh.loadMat4("model", transformationMatrix);
	sh.loadVec2("texScaling", texScaling);

	sh.loadBool("material.hasTexture", false);
	sh.loadBool("material.hasNormalMap", false);
	sh.loadBool("material.hasMetallicTex", false);

	sh.loadFloat("material.shininess", shininess);

	sh.loadVec3("material.diffuse", diffuseColor);
	sh.loadVec3("material.specular", specularColor);

	for (auto &subObject : subObjects)
	{
		glBindVertexArray(subObject.vao);

		glDrawElements(GL_TRIANGLES, subObject.indices.size(), GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
	}
}
