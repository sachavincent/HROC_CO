#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <vector>
#include <glm/common.hpp>
#include <iostream>
#include <glad/glad.h>

class Texture
{
public:
    Texture(const std::string& file);

    ~Texture();

	void load();
	
	void unload();

    void cleanUp();

    static Texture& DEFAULT()
    {
        static Texture tex("default.png");
    	return tex;
    }
public:
	static const std::string DIR_PATH;
private:
    GLuint _id;
};

#endif // TEXTURE_H
