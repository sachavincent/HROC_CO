#ifndef SHADER_H
#define SHADER_H

#include <glm/common.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glad/glad.h>

class Shader
{
public:
    Shader(std::string vertexPath, std::string fragmentPath);

    void start();
    void stop();
    void cleanUp();

    void loadBool(const std::string& name, bool value) const;  
    void loadInt(const std::string& name, int value) const;   
    void loadFloat(const std::string& name, float value) const;
    void loadMatrix4(const std::string& name, glm::f32* value, int nb = 1) const;
    void loadMatrix2x4(const std::string& name, glm::mat2x4 value, int nb = 1) const;
    void loadVec3(const std::string& name, glm::vec3 value) const;
private:
    unsigned int ID;

    bool _started, _deleted;

    GLuint _vertexID;
    GLuint _fragmentID;

    static const std::string SHADER_PATH;
};

#endif // SHADER_H
