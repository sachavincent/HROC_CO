#include "shader.hpp"

std::map<std::pair<std::string, std::string>, int> Shader::buffer = {};

const char *Shader::loadShader(std::string path)
{
	path = Utils::workingDirectory() + path;

	std::ifstream file(path);

	std::string str((std::istreambuf_iterator<char>(file)),
					std::istreambuf_iterator<char>());
	int fSize = str.length();

	char *cstr = new char[fSize + 1];

	if (fSize == 0)
	{
		std::cout << "failed to load shader with path : " << path << std::endl;
	}
	strcpy(cstr, str.c_str());
	file.close();

	return cstr;
}

Shader::Shader(std::string computePath)
{
	std::pair<std::string, std::string> paths = {computePath, ""};
	auto it = buffer.find(paths);
	if (it != buffer.end())
	{
		ID = it->second;
		std::cout << it->second << std::endl;
		return;
	}

	const char *source = Shader::loadShader(computePath);
	// shader creation and compilation
	unsigned int shader;
	shader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	// shader program creation
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, shader);
	glLinkProgram(shaderProgram);
	// shader program creation
	glUseProgram(shaderProgram);

	// temp shader deletion
	glDeleteShader(shader);

	ID = shaderProgram;
	buffer[paths] = ID;
}

Shader::Shader(std::string vertexPath, std::string fragmentPath)
{
	std::pair<std::string, std::string> paths = {vertexPath, fragmentPath};
	auto it = buffer.find(paths);
	if (it != buffer.end())
	{
		ID = it->second;
		std::cout << it->second << std::endl;
		return;
	}

	const char *vertexShaderSource = Shader::loadShader(vertexPath);
	const char *fragmentShaderSource = Shader::loadShader(fragmentPath);

	// vertex shader creation and compilation
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// fragment shader creation and compilation
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// shader program creation
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// shader program creation
	glUseProgram(shaderProgram);
	// temp shader deletion
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	ID = shaderProgram;
	buffer[paths] = ID;
}

void Shader::start()
{
	glUseProgram(ID);
}

void Shader::stop()
{
	glUseProgram(0);
}

void Shader::loadBool(const std::string &name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::loadInt(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::loadFloat(const std::string &name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::loadMat4(const std::string &name, glm::mat4 value) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}
void Shader::loadVec3(const std::string &name, glm::vec3 value) const
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
}
void Shader::loadVec2(const std::string &name, glm::vec2 value) const
{
	glUniform2f(glGetUniformLocation(ID, name.c_str()), value.x, value.y);
}