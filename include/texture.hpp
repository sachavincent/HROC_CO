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

        // Create storage for the texture. (100 layers of 1x1 texels)
        glTexStorage3D(GL_TEXTURE_2D_ARRAY,
                       1,
                       GL_RGBA16F,    // Internal format
                       width, height, // width,height
                       maxObjects     // Number of layers
        );
        arrayInit = true;
    }
    static GLuint loadTexture(const std::string &_file, unsigned int _id);

    static void load();

    static void unload();

    inline static const GLuint &getId() { return id; }
};

#endif