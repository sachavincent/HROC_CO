#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <stb_image.h>
#include <glad/glad.h>
#include <map>
#include <string>
#include <vector>
#include <iostream>

#include "utils/utils.hpp"

class Texture
{
private:
    static std::map<std::string, GLuint> cache;

    static GLuint id;
    static bool arrayInit;
    static unsigned int maxObjects;
    static int width;
    static int height;
    static unsigned int currObj;

public:
    static void createTextureArray(unsigned int nbObjects, unsigned int _width, unsigned int _height)
    {
        maxObjects = nbObjects;

        width = _width;
        height = _height;
        glGenTextures(1, &id);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, id);

        glTexStorage3D(GL_TEXTURE_2D_ARRAY,
                       1,
                       GL_RGBA16F,    // Internal format
                       width, height, // width,height
                       maxObjects     // Number of layers
        );

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        arrayInit = true;
    }

    static GLuint loadTexture(const std::string &_file, unsigned int _id);

    static void load();

    static void unload();

    inline static const GLuint &getId() { return id; }

    static void cleanUp()
    {
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        glDeleteTextures(1, &id);
        Texture::cache.clear();
        arrayInit = false;
        currObj = 0;
    }
};

#endif