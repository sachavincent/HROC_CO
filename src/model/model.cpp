#include "model.hpp"
#include "scene.hpp"
#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"
#include <algorithm>

void Model::load()
{

	// gen geometry buffers
	glGenBuffers(1, &m.vbo);
	glGenBuffers(1, &m.nbo);
	glGenBuffers(1, &m.tbo);
	glGenBuffers(1, &m.ebo);
	glGenVertexArrays(1, &m.vao);

	// Bind the vao
	glBindVertexArray(m.vao);

	// create and fill vertex data
	glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
	glBufferData(GL_ARRAY_BUFFER, m.vertices.size() * sizeof(GLfloat), m.vertices.data(), GL_STATIC_DRAW);
	// set vertex attribute pointer
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
	glEnableVertexAttribArray(0);

	// copy normals to nbo
	glBindBuffer(GL_ARRAY_BUFFER, m.nbo);
	glBufferData(GL_ARRAY_BUFFER, m.normals.size() * sizeof(GLfloat), m.normals.data(), GL_STATIC_DRAW);
	// define array for normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
	glEnableVertexAttribArray(1);

	// Copy texture array in element buffer
	glBindBuffer(GL_ARRAY_BUFFER, m.tbo);
	glBufferData(GL_ARRAY_BUFFER, m.textureCoord.size() * sizeof(GLfloat), m.textureCoord.data(), GL_STATIC_DRAW);
	// define array for texture
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
	glEnableVertexAttribArray(2);

	//copy indices to ebo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m.indices.size() * sizeof(GLfloat), m.indices.data(), GL_STATIC_DRAW);

	// Unbind the VAO
	glBindVertexArray(0);

	// load textures if defined
	if (m.diffuseMapPath != "")
	{
		m.diffuseMap = Texture::loadTexture(m.diffuseMapPath.c_str());
	}
	if (m.specularMapPath != "")
	{
		m.specularMap = Texture::loadTexture(m.specularMapPath.c_str());
	}
	loadShaders();
}

void Model::render(Scene *_scene)
{
	Camera &cam = _scene->getCamera();
	auto &lights = _scene->getLights();

	m.shader.start();
	//glm::vec3 axis{0,1,0};
	//m.rotation = glm::rotate(m.rotation,glm::radians(0.01f),axis);
	glm::mat4 model = m.translate * m.rotation * m.scale;

	m.shader.loadMat4("model", model);
	m.shader.loadMat4("view", cam.getViewMatrix());
	m.shader.loadMat4("projection", cam.getProjectionMatrix());
	m.shader.loadVec2("screenSize", glm::vec2(cam.getResWidth(), cam.getResHeight()));
	m.shader.loadVec3("viewPos", cam.getPosition());
	m.shader.loadFloat("exposure", _scene->getExposure());
	m.shader.loadVec2("texScaling", m.texScaling);

	m.shader.loadFloat("material.specularStrength", 0.5f);
	m.shader.loadFloat("material.shininess", m.shininess);

	// bind diffuse/albedo texture
	glActiveTexture(GL_TEXTURE0);
	if (m.diffuseMap != Texture::DEFAULT_TEXTURE())
	{
		m.shader.loadInt("material.diffuseTex", 0);
		m.shader.loadBool("material.hasTexture", true);
		m.diffuseMap.load();
	}
	else
	{
		m.shader.loadVec3("material.diffuse", m.diffuseColor);
		m.shader.loadBool("material.hasTexture", false);
		m.diffuseMap.unload();
	}
	// bind specular/roughness texture
	glActiveTexture(GL_TEXTURE1);
	if (m.specularMap != Texture::DEFAULT_TEXTURE())
	{
		m.shader.loadInt("material.specularTex", 1);
		m.specularMap.load();
	}
	else
	{
		m.shader.loadVec3("material.specular", m.specularColor);
		m.specularMap.unload();
	}
	//bind mellatlic texture (if PBR)
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);

	for (uint32_t i = 0; i < std::min(lights.size(), (size_t)MAXLIGHTS); i++)
	{
		m.shader.loadBool("lights[" + std::to_string(i) + "].enabled", 1);

		m.shader.loadVec3("lights[" + std::to_string(i) + "].position", lights[i]->getPosition());

		m.shader.loadVec3("lights[" + std::to_string(i) + "].color", lights[i]->getColor());
		m.shader.loadVec3("lights[" + std::to_string(i) + "].attenuation", lights[i]->getAttenuation());
	}
	if (lights.size() < MAXLIGHTS)
	{
		for (size_t i = lights.size(); i < MAXLIGHTS; i++)
		{
			m.shader.loadBool("lights[" + std::to_string(i) + "].enabled", 0);
		}
	}

	glBindVertexArray(m.vao);

	glDrawElements(GL_TRIANGLES, m.indices.size(), GL_UNSIGNED_INT, nullptr);

	glBindVertexArray(0);
	m.shader.stop();
}

void Model::loadShaders()
{
	m.shader = {"shaders/default.vert", "shaders/phong.frag"};
}

Model &Model::setScale(glm::vec3 _scale)
{
	m.scale = glm::mat4(1.0);
	m.scale = glm::scale(m.scale, _scale);
	return *this;
}

Model &Model::setRotation(float _angle, glm::vec3 _axis)
{
	m.rotation = glm::mat4(1.0);
	m.rotation = glm::rotate(m.rotation, glm::radians(_angle), _axis);
	return *this;
}

Model &Model::setPosition(glm::vec3 _pos)
{
	m.translate = glm::mat4{1.0};
	m.translate = glm::translate(m.translate, _pos);
	return *this;
}

Model &Model::setTexDiffuse(std::string _path)
{
	m.diffuseMapPath = _path;
	return *this;
}

Model &Model::setTexSpecular(std::string _path)
{
	m.specularMapPath = _path;
	return *this;
}

Model &Model::setShininess(float _shininess)
{
	m.shininess = _shininess;
	return *this;
}

Model &Model::setDiffuse(glm::vec3 _color)
{
	m.diffuseColor = _color;
	return *this;
}

Model &Model::setSpecular(glm::vec3 _color)
{
	m.specularColor = _color;
	return *this;
}

Model &Model::setTexScaling(glm::vec2 _scale)
{
	m.texScaling = _scale;
	return *this;
}
