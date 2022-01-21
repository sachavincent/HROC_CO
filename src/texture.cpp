#include "texture.hpp"

std::map<std::string, int> Texture::cache = {};

Texture::Texture(const std::string &file)
{
    std::string path = "" + file;
    //load a texture only if it has not been loaded previously (avoids loading duplicates)
    if (Texture::cache.find(path) == Texture::cache.end())
    {
        glGenTextures(1, &_id);
        Texture::cache[path] = _id;
    }
    else
    {
        _id = Texture::cache.find(path)->second;
    }

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
    if (!data)
    {
        std::cerr << "Cannot load file image: " << path.c_str() << ", STB Reason: " << stbi_failure_reason() << std::endl;
        return;
    }

    size_t numPixels = width * height * nrComponents;

    std::vector<float> dataf;
    dataf.reserve(width * height * 4);
    if (nrComponents == 4)
    {
        for (size_t i = 0; i < numPixels; i++)
        {
            dataf.push_back((float)(data[i] - '0') / 255);
        }
    }
    else if (nrComponents == 3)
    {
        for (size_t i = 0; i < numPixels; i += 3)
        {
            dataf.push_back((float)(data[i] - '0') / 255);
            dataf.push_back((float)(data[i + 1] - '0') / 255);
            dataf.push_back((float)(data[i + 2] - '0') / 255);
            dataf.push_back(1.0);
        }
    }
    else if (nrComponents == 1)
    {
        for (size_t i = 0; i < numPixels; i++)
        {
            dataf.push_back((float)(data[i] - '0') / 255);
            dataf.push_back((float)(data[i] - '0') / 255);
            dataf.push_back((float)(data[i] - '0') / 255);
            dataf.push_back(1);
        }
    }

    if (data)
    {
        GLenum format = GL_RGBA16F;
        GLenum iformat = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, _id);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, iformat, GL_FLOAT, &dataf[0]);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
}

Texture Texture::loadTexture(const std::string &file)
{
    return Texture(file);
}

void Texture::load()
{
    glBindTexture(GL_TEXTURE_2D, _id);
}

void Texture::unload()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}