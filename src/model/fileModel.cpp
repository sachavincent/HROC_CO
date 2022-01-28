#include "model.hpp"
#include "scene.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

int FileModel::instance = 0;

FileModel::FileModel(std::string _path, SMOOTH_NORMAL _smoothNormals)
{
	name = _path.substr(_path.find_last_of("/") + 1) + "_" + std::to_string(instance);
	instance++;

	Assimp::Importer importer;

	std::cout << "loading model from file : "
			  << _path << " ..." << std::endl;

	const aiScene *scene = _smoothNormals ? 
	importer.ReadFile(_path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals)
	  : importer.ReadFile(_path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	// std::string directory = _path.substr(0, _path.find_last_of('/'));

	subModels.resize(scene->mNumMeshes);
	// process each mesh of the model

	std::cout << "processing " << subModels.size() << " submeshes ..." << std::endl;

	// configure each mesh
	for (size_t i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[i];
		subModels[i].translate = glm::mat4{1.0};
		subModels[i].scale = glm::mat4{1.0};
		subModels[i].rotation = glm::mat4{1.0};
		processMesh(mesh, scene, i);
	}

	// center objects vertices (for scaling later)
	double meanX = 0, meanY = 0, meanZ = 0;
	size_t count = 0;
	for (auto &subModel : subModels)
	{
		for (size_t i = 0; i < subModel.vertices.size(); i += 3)
		{
			count++;
			meanX += subModel.vertices[i];
			meanY += subModel.vertices[i + 1];
			meanZ += subModel.vertices[i + 2];
		}
	}

	meanX /= count;
	meanY /= count;
	meanZ /= count;
	for (auto &subModel : subModels)
	{
		for (size_t i = 0; i < subModel.vertices.size(); i += 3)
		{
			count++;
			subModel.vertices[i] -= (GLfloat)meanX;
			subModel.vertices[i + 1] -= (GLfloat)meanY;
			subModel.vertices[i + 2] -= (GLfloat)meanZ;
		}
	}
}

void FileModel::processMesh(aiMesh *_mesh, const aiScene *_scene, size_t _meshIdx)
{
	auto &vert = subModels[_meshIdx].vertices;
	auto &ind = subModels[_meshIdx].indices;
	auto &norm = subModels[_meshIdx].normals;
	// auto& tex = subModels[_meshIdx].textureCoord;

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
void FileModel::load()
{
	for (auto &subModel : subModels)
	{

		// gen geometry buffers
		glGenBuffers(1, &subModel.vbo);
		glGenBuffers(1, &subModel.nbo);
		glGenBuffers(1, &subModel.tbo);
		glGenBuffers(1, &subModel.ebo);
		glGenVertexArrays(1, &subModel.vao);

		// Bind the vao
		glBindVertexArray(subModel.vao);

		// copy indices to vbo
		glBindBuffer(GL_ARRAY_BUFFER, subModel.vbo);
		glBufferData(GL_ARRAY_BUFFER, subModel.vertices.size() * sizeof(GLfloat), subModel.vertices.data(), GL_STATIC_DRAW);
		// define array for vertices
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
		glEnableVertexAttribArray(0);

		// copy normals to nbo
		glBindBuffer(GL_ARRAY_BUFFER, subModel.nbo);
		glBufferData(GL_ARRAY_BUFFER, subModel.normals.size() * sizeof(GLfloat), subModel.normals.data(), GL_STATIC_DRAW);
		// define array for normals
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
		glEnableVertexAttribArray(1);

		//// Copy texture coordinates array in element buffer
		// glBindBuffer(GL_ARRAY_BUFFER, subModel.tbo);
		// glBufferData(GL_ARRAY_BUFFER, subModel.textureCoord.size() * sizeof(GLfloat), subModel.textureCoord.data(), GL_STATIC_DRAW);
		//// define array for texture coordinates
		// glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
		// glEnableVertexAttribArray(2);

		// copy indices to ebo
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subModel.ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, subModel.indices.size() * sizeof(GLfloat), subModel.indices.data(), GL_STATIC_DRAW);

		// Unbind the VAO
		glBindVertexArray(0);

		// load right shader

		if ((subModel.diffuseMapPath != ""))
		{
			subModel.diffuseMap = Texture::loadTexture(subModel.diffuseMapPath.c_str());
		}
		if (subModel.specularMapPath != "")
		{
			subModel.specularMap = Texture::loadTexture(subModel.specularMapPath.c_str());
		}
		loadShaders(subModel);
	}
}

void FileModel::render(Scene *_scene)
{
	Camera *cam = _scene->getCamera();
	auto &lights = _scene->getLights();
	for (auto &subModel : subModels)
	{

		auto &sh = subModel.shader;
		sh.start();

		glm::mat4 model = subModel.translate * subModel.rotation * subModel.scale;

		sh.loadMat4("model", model);
		sh.loadMat4("view", cam->getViewMatrix());
		sh.loadMat4("projection", cam->getProjectionMatrix());
		sh.loadVec2("screenSize", glm::vec2(cam->getResWidth(), cam->getResHeight()));
		sh.loadVec3("viewPos", cam->getPosition());
		sh.loadFloat("exposure", _scene->getExposure());
		sh.loadVec2("texScaling", subModel.texScaling);

		sh.loadBool("material.hasTexture", false);
		sh.loadBool("material.hasNormalMap", false);
		sh.loadBool("material.hasMetallicTex", false);

		sh.loadFloat("material.specularStrength", 1.0f);
		sh.loadFloat("material.shininess", subModel.shininess);
		sh.loadVec3("material.diffuse", subModel.diffuseColor);
		sh.loadVec3("material.specular", subModel.specularColor);

		for (uint32_t i = 0; i < std::min(lights.size(), (size_t)MAXLIGHTS); i++)
		{
			sh.loadBool("lights[" + std::to_string(i) + "].enabled", 1);
			sh.loadVec3("lights[" + std::to_string(i) + "].position", lights[i]->getPosition());
			sh.loadVec3("lights[" + std::to_string(i) + "].color", lights[i]->getColor());
			sh.loadVec3("lights[" + std::to_string(i) + "].attenuation", lights[i]->getAttenuation());
		}
		if (lights.size() < MAXLIGHTS)
		{
			for (size_t i = lights.size(); i < MAXLIGHTS; i++)
			{
				sh.loadBool("lights[" + std::to_string(i) + "].enabled", 0);
			}
		}

		glBindVertexArray(subModel.vao);

		glDrawElements(GL_TRIANGLES, subModel.indices.size(), GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
		sh.stop();
	}
}

void FileModel::loadShaders(modelDescription &model)
{

	model.shader = {"shaders/default.vert", "shaders/phong.frag"};
}

void FileModel::loadShaders()
{
	for (auto &subModel : subModels)
		loadShaders(subModel);
}

FileModel &FileModel::setScale(glm::vec3 _scale)
{
	for (auto &subModel : subModels)
	{
		subModel.scale = glm::mat4(1.0);
		subModel.scale = glm::scale(subModel.scale, _scale);
	}
	return *this;
}

FileModel &FileModel::setRotation(float _angle, glm::vec3 _axis)
{
	for (auto &subModel : subModels)
	{
		subModel.rotation = glm::mat4(1.0);
		subModel.rotation = glm::rotate(subModel.rotation, glm::radians(_angle), _axis);
	}
	return *this;
}

FileModel &FileModel::setPosition(glm::vec3 _pos)
{
	for (auto &subModel : subModels)
	{
		subModel.translate = glm::mat4{1.0};
		subModel.translate = glm::translate(subModel.translate, _pos);
	}
	return *this;
}

FileModel &FileModel::setDiffuse(glm::vec3 _color)
{
	for (auto &subModel : subModels)
	{
		subModel.diffuseColor = _color;
	}
	return *this;
}

FileModel &FileModel::setSpecular(glm::vec3 _color)
{
	for (auto &subModel : subModels)
	{
		subModel.specularColor = _color;
	}
	return *this;
}

FileModel &FileModel::setShininess(float _shininess)
{
	for (auto &subModel : subModels)
	{
		subModel.shininess = _shininess;
	}
	return *this;
}
