#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#ifndef GLM_H
#define GLM_H
#include <glm/glm.hpp>
#endif

#include <fstream>
#include <string>
#include <iostream>


enum SHADER_TYPE{
    PHONG,
    PBR
};

class Shader {
private:
	const char* loadShader(std::string path);

public:
	unsigned int ID;
	Shader(){};
	// with tesselation shader
	Shader(std::string vertexPath, std::string fragmentPath,
		std::string tessControlPath, std::string tessEvalPath);
	// without tesselation
	Shader(std::string vertexPath, std::string fragmentPath);

	void use();

	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;
	void setMat4(const std::string &name, glm::mat4 value) const;
	void setVec3(const std::string& name, glm::vec3 value) const;
	void setVec2(const std::string& name, glm::vec2 value) const;
};


#endif