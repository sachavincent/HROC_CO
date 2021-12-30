#include "shader.h"

const std::string Shader::SHADER_PATH = "../../../../src/shaders/";

Shader::Shader(std::string vertexPath, std::string fragmentPath): _started(false), _deleted(false) {
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        vShaderFile.open(Shader::SHADER_PATH + vertexPath, std::ios::binary);
        fShaderFile.open(Shader::SHADER_PATH + fragmentPath, std::ios::binary);
        std::stringstream vShaderStream, fShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode   = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    } catch(std::ifstream::failure e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

	int success;
	char infoLog[512];
	
	this->_vertexID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(this->_vertexID, 1, &vShaderCode, NULL);
    glCompileShader(this->_vertexID);

    glGetShaderiv(this->_vertexID, GL_COMPILE_STATUS, &success);
	if(!success) {
        glGetShaderInfoLog(this->_vertexID, 512, NULL, infoLog);
	    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	
    this->_fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(this->_fragmentID, 1, &fShaderCode, NULL);
    glCompileShader(this->_fragmentID);
    glGetShaderiv(this->_fragmentID, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(this->_fragmentID, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
	
	this->ID = glCreateProgram();
    glAttachShader(this->ID, this->_vertexID);
    glAttachShader(this->ID, this->_fragmentID);
    glLinkProgram(this->ID);

    glGetProgramiv(this->ID, GL_LINK_STATUS, &success);
	if(!success) {
        glGetProgramInfoLog(this->ID, 512, NULL, infoLog);
	    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	
    glDeleteShader(this->_vertexID);
    glDeleteShader(this->_fragmentID);
}

void Shader::start() 
{
    if(!this->_started)
    {
        this->_started = true;
        glUseProgram(this->ID);
    }
}

void Shader::stop() 
{
    if(this->_started)
    {
        this->_started = false;
        glUseProgram(0);
    }
}

void Shader::cleanUp() 
{
    if(_deleted)
        return;

    stop();

    glDetachShader(this->ID, this->_vertexID);
    glDetachShader(this->ID, this->_fragmentID);

    glDeleteShader(this->_vertexID);
    glDeleteShader(this->_fragmentID);

    glDeleteProgram(this->ID);

    this->_deleted = true;
}


void Shader::loadInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(this->ID, name.c_str()), value);
}

void Shader::loadBool(const std::string& name, bool value) const
{         
    loadInt(name, (int) value);
}

void Shader::loadFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(this->ID, name.c_str()), value);
}

void Shader::loadMatrix4(const std::string& name, glm::f32* value, int nb) const
{
    glUniformMatrix4fv(glGetUniformLocation(this->ID, name.c_str()), nb, GL_FALSE, value);
}

void Shader::loadMatrix2x4(const std::string& name, glm::mat2x4 value, int nb) const
{
    glUniformMatrix2x4fv(glGetUniformLocation(this->ID, name.c_str()), nb, GL_FALSE, &value[0][0]);
}

void Shader::loadVec3(const std::string& name, glm::vec3 value) const
{
    glUniform3f(glGetUniformLocation(this->ID, name.c_str()), value[0], value[1], value[2]);
}
