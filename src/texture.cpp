#include "texture.h"

#include <stb_image.h>
#include <iostream>

const std::string Texture::DIR_PATH = "../../../../assets/";

Texture::Texture(const std::string& file)
{
    int width, height, nrChannels;
    std::string str = Texture::DIR_PATH + file;
    stbi_uc* data = stbi_load(str.c_str(), &width, &height, &nrChannels, 4);
    glGenTextures(1, &this->_id);

    load();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 3);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);

    unload();
}

Texture::~Texture()
{
}

void Texture::load()
{
    glBindTexture(GL_TEXTURE_2D, this->_id);
}

void Texture::unload()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::cleanUp()
{
    glDeleteTextures(1, &this->_id);
}
