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
    Texture(int id) : _id(id) {}
    Texture(const std::string &file);

public:
    static Texture &DEFAULT_TEXTURE()
    {
        static Texture tex("default.png");
        return tex;
    }

    static Texture loadTexture(const std::string &file);

    void load();

    void unload();

    inline const GLuint &getId() const { return _id; }

    bool operator==(const Texture &t)
    {
        return t.getId() == _id;
    }

    bool operator!=(const Texture &t)
    {
        return t.getId() != _id;
    }

private:
    static std::map<std::string, int> cache;

    GLuint _id;
};

#endif