#ifndef TEXTURE_HPP
#define TEXTURE_HPP
#include <stb_image.h>
#include <glad/glad.h>
#include <map>
#include <string>
#include <vector>
#include <iostream>

class Texture
{
private:
    Texture(int id) : _id(id) {}

public:
    static Texture loadTexture(const std::string &file);

    void load();

    void unload();

    inline const int &getId() const { return _id; }

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

    int _id;

public:
    static Texture DEFAULT_TEXTURE;
};

#endif