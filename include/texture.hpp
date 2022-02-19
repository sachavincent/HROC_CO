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
    static std::map<std::string, std::vector<float>> cache;

    static GLuint id;
    static bool arrayInit;
    static unsigned int currObj;
    static unsigned int maxObjects;

public:
    static void createTextureArray(unsigned int nbObjects)
    {
        maxObjects = nbObjects;

        glGenTextures(1, &id);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, id);

        // Create storage for the texture. (100 layers of 1x1 texels)
        glTexStorage3D(GL_TEXTURE_2D_ARRAY,
                       1,
                       GL_RGBA16F, // Internal format
                       1024, 1024, // width,height
                       maxObjects  // Number of layers
        );
        arrayInit = true;
    }
    static void loadTexture(const std::string &_file, unsigned int _id);

    static void load();

    static void unload();

    inline static const GLuint &getId() { return id; }
};

#endif