#ifndef UTILS_H
#define UTILS_H

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include <stb_image.h>
#include <stb_image_write.h>
#endif
#include <glad/glad.h>

#include<stdlib.h>

#include <iostream>
#include <map>

static std::map<std::string,unsigned int> texturePathMap;

static unsigned int loadTexture(const char* path) {
    unsigned int textureID;

    //load a texture only if it has not been loaded previously (avoids loading duplicates)
    if (texturePathMap.find(path) == texturePathMap.end()){
        glGenTextures(1, &textureID);
        texturePathMap[path] = textureID;
    } else {
        return texturePathMap.find(path)->second;
    }


    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (!data) {
        fprintf(stderr, "Cannot load file image %s\nSTB Reason: %s\n", path, stbi_failure_reason());
        exit(0);
    }
    
    size_t numPixels = width*height*nrComponents;

    std::vector<float> dataf;
    dataf.reserve(width*height*4);
    if(nrComponents == 4){
        for(size_t i = 0; i<numPixels ;i++){
            dataf.push_back((float)(data[i]-'0')/255);
        }
    } else if (nrComponents == 3){
        for(size_t i = 0; i<numPixels ;i+=3){
            dataf.push_back((float)(data[i]-'0')/255);
            dataf.push_back((float)(data[i+1]-'0')/255);
            dataf.push_back((float)(data[i+2]-'0')/255);
            dataf.push_back(1.0);
        }
    }else if (nrComponents == 1){
        for(size_t i = 0; i<numPixels ;i++){
            dataf.push_back((float)(data[i]-'0')/255);
            dataf.push_back((float)(data[i]-'0')/255);
            dataf.push_back((float)(data[i]-'0')/255);
            dataf.push_back(1);
        }
    }

    if (data) {
        GLenum format = GL_RGBA16F;
        GLenum iformat = GL_RGBA;
            

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, iformat,
                     GL_FLOAT, &dataf[0]);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    } else {
        std::cout
            << "Texture failed to load at path: " << path << std::endl
            << "For more info, check CubeMap's documentation on file naming."
            << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

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