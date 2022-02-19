#ifndef UTILS_H
#define UTILS_H

#include <string>
#ifndef WIN32
#include <libgen.h>
#include <unistd.h>
#include <linux/limits.h>
#else
#include <Windows.h>
#endif
#include <glm/glm.hpp>

#include <glad/glad.h>
struct OBJECT_BOUNDS
{
    glm::vec3 max;
    glm::vec3 min;
};
struct Vertex
{
    glm::vec3 Position;
    glm::vec2 TexCoord;
    glm::vec3 Normal;
};
struct OBJECT_DATA
{
    Vertex *vertices;
    GLuint *indices;
    unsigned int numVertices, numIndices;
    unsigned int numInstances;

    // LOCAL object maximum and minimum bounds without transformation
    OBJECT_BOUNDS bounds;
};
struct DrawElementsCommand
{
    GLuint vertexCount;
    GLuint instanceCount;
    GLuint firstIndex;
    GLuint baseVertex;
    GLuint baseInstance;
};
class Utils
{
public:
    static std::string workingDirectory()
    {
        char workingDir[256];
#ifndef WIN32
        ssize_t count = readlink("/proc/self/exe", workingDir, PATH_MAX);
        if (count)
            return std::string(dirname(workingDir));
        return "";
#else
        GetModuleFileNameA(NULL, workingDir, 256);
        std::string::size_type pos = std::string(workingDir).find_last_of("\\/");
        return std::string(workingDir).substr(0, pos + 1);
#endif
    }
};

#endif