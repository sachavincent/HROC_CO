#ifndef UTILS_H
#define UTILS_H

#include <stb_image.h>
#include <stb_image_write.h>
#include <glad/glad.h>

#include<stdlib.h>

#include <iostream>
#include <map>

static std::map<std::string,unsigned int> texturePathMap;

static unsigned int loadCubeMapTexture(std::string _directory) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    std::string texnames[6]{"right", "left", "top", "bottom", "front", "back"};

    std::string exts[3]{".png", ".jpg", ".jpeg"};
    std::string fname;
    unsigned char* data;

    int width, height, nrComponents;
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 3; j++) {  // check extension
            fname = _directory + texnames[i] + exts[j];
            data = stbi_load(fname.c_str(), &width, &height, &nrComponents, 0);
            if (data) {
                break;
            }
        }
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, width,
                         height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

            stbi_image_free(data);
        } else {
            std::cout << "Texture failed to load at path: " << fname
                      << std::endl;
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    return textureID;
}

#endif