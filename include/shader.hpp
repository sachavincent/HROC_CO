#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#ifndef GLM_H
#define GLM_H
#include <glm/glm.hpp>
#endif

#include <fstream>
#ifdef NOT_MSVC
#include <string.h>
#endif
#include <string>
#include <iostream>

enum SHADER_TYPE
{
	PHONG,
	PBR
};

class Shader
{
private:
	static const char *loadShader(std::string path);

public:
	unsigned int ID;
	Shader() {}
	Shader(std::string computePath);
	Shader(std::string vertexPath, std::string fragmentPath);

	void start();
	void stop();

	void loadBool(const std::string &name, bool value) const;
	void loadInt(const std::string &name, int value) const;
	void loadFloat(const std::string &name, float value) const;
	void loadMat4(const std::string &name, glm::mat4 value) const;
	void loadVec3(const std::string &name, glm::vec3 value) const;
	void loadVec2(const std::string &name, glm::vec2 value) const;
};

#endif