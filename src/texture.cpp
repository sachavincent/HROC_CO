#include "texture.hpp"

std::map<std::string, GLuint> Texture::cache;

GLuint Texture::id;
bool Texture::arrayInit = false;
unsigned int Texture::maxObjects = 0;
unsigned int Texture::width = 0;
unsigned int Texture::height = 0;
unsigned int Texture::currObj = 0;

GLuint Texture::loadTexture(const std::string &_file, unsigned int _id)
{
#ifndef HROC_TESTS
    if (currObj > maxObjects)
    {
        std::cerr << "Texture array size too small! (" << _id << " < " << maxObjects << ")" << std::endl;
        return 0;
    }

    if (!arrayInit)
    {
        std::cerr << "Texture array not initialized!" << std::endl;
        return 0;
    }

    std::string path = Utils::workingDirectory() + _file;

    // load a texture only if it has not been loaded previously (avoids loading duplicates)
    if (Texture::cache.find(path) == Texture::cache.end())
    {
        int _width, _height, nrComponents;
        unsigned char *data = stbi_load(path.c_str(), &_width, &_height, &nrComponents, 0);
        if (!data)
        {
            std::cerr << "Cannot load file image: " << path.c_str() << ", STB Reason: " << stbi_failure_reason() << std::endl;
            return 0;
        }
        if (width != _width && height != _height)
        {
            std::cerr << "Textures must be 1024x1024!" << std::endl;
            return 0;
        }
        size_t numPixels = _width * _height * nrComponents;

        std::vector<float> dataf;
        dataf.reserve(_width * _height * 4);
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
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
                        0,                // Mipmap number
                        0, 0, currObj,        // xoffset, yoffset, zoffset
                        width, height, 1, // width, height, depth
                        GL_RGBA,          // format
                        GL_FLOAT,         // type
                        &dataf[0]);       // pointer to data

        Texture::cache[path] = currObj++;
    }
#endif

    return Texture::cache[path];
}

void Texture::load()
{
    glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::unload()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}