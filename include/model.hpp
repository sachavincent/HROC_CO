#ifndef MODEL_H
#define MODEL_H

#define MAXLIGHTS 10

#include <vector>
#include <string>
#include <fstream>

#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "shader.hpp"
#include "light.hpp"
#include "camera.hpp"
#include "texture.hpp"

typedef enum {
    SMOOTH_NORMAL_ENABLE = true,
    SMOOTH_NORMAL_DISABLE = false
} SMOOTH_NORMAL;

class Scene;

class Model{
protected:
    //! une structure qui encapsule la description d'un modèle
    struct modelDescription{
        // The name of the object in the UI.
        std::string name;
        // vbo = vertices
        // nbo = normals
        // ebo = indices
        // tbo = texture coord
        unsigned int vbo, nbo, ebo, tbo, vao;

        // shading
        Shader shader;
        bool shaderLoaded = false;

        std::string shaderVertPath = "";
        std::string shaderFragPath = "";

        Texture diffuseMap = Texture::DEFAULT_TEXTURE();
        Texture specularMap = Texture::DEFAULT_TEXTURE();

        std::string diffuseMapPath = "";
        std::string specularMapPath = "";

        glm::vec3 diffuseColor = glm::vec3{0.9};
        glm::vec3 specularColor = glm::vec3{1.0};
        float shininess = 64.0f;

        glm::vec2 texScaling = {1,1};

        std::vector<GLfloat> vertices, normals, textureCoord;
        std::vector<GLuint>  indices;

        glm::mat4 scale, rotation, translate;
    };

    modelDescription m;
    
public:
    Model(){};

    //! Load the object on the gpu. This action is performed after opengl/glfw initialization
    virtual void load();

    virtual void loadShaders();

    //! Render the object on screen.
    virtual void render(Scene* _scene);

    virtual Model& setScale(glm::vec3 _scale);
    virtual Model& setRotation(float _angle, glm::vec3 _axis);
    virtual Model& setPosition(glm::vec3 _pos);

    Model& setTexDiffuse(std::string _path);
    Model& setTexSpecular(std::string _path);

    virtual Model& setTexScaling( glm::vec2 _scale);

    //! Set color of object (unused if textures are defined)
    virtual Model& setDiffuse(glm::vec3 _color);
    virtual Model& setSpecular(glm::vec3 _color);
    virtual Model& setShininess(float _shininess);

    virtual glm::vec3 getPosition(){return glm::vec3(m.translate[3]);}
    virtual glm::vec3 getScale(){return glm::vec3(m.scale[0][0],m.scale[1][1],m.scale[2][2]);}

    virtual glm::vec3 getDiffuse(){return m.diffuseColor;}
    virtual glm::vec3 getSpecular(){return m.specularColor;}
    virtual float getShininess() {return m.shininess;}

    virtual glm::vec2 getTexScaling(){return m.texScaling;}

    virtual std::string getName() = 0;
    virtual bool hasTextures(){return m.diffuseMap != Texture::DEFAULT_TEXTURE();} 
};


//! a simple cube to test shader on
class Cube : public Model{
    static int instance;
public:
    Cube(){};
    std::string getName(){return m.name;}
    //! Create a cube of size _edgeSize.
    Cube(float _edgeSize);
};


//! A model loaded from a file, it can contain multiple subModels inside it.
class FileModel : public Model{
private:
    std::string name;
    static int instance;
    std::vector<modelDescription> subModels;

    void loadShaders(modelDescription& model);
    void processMesh(aiMesh *_mesh, const aiScene *_scene, size_t _meshIdx);
public:
    FileModel(std::string _path, SMOOTH_NORMAL _smoothNormals);
    void load();
    void loadShaders();
    void render(Scene* _scene);

    FileModel& setScale(glm::vec3 _scale);
    FileModel& setRotation(float _angle, glm::vec3 _axis);
    FileModel& setPosition(glm::vec3 _pos);

    FileModel& setDiffuse(glm::vec3 _color);
    FileModel& setSpecular(glm::vec3 _color);
    FileModel& setShininess(float _shininess);

    glm::vec3 getPosition(){return glm::vec3(subModels[0].translate[3]);}
    glm::vec3 getScale(){return glm::vec3(subModels[0].scale[0][0],
                                        subModels[0].scale[1][1]
                                        ,subModels[0].scale[2][2]);}

    glm::vec3 getDiffuse(){return subModels[0].diffuseColor;}
    glm::vec3 getSpecular(){return subModels[0].specularColor;}
    float getShininess() {return subModels[0].shininess;}

    std::string getName(){return name;}
    bool hasTextures(){return false;}

};

class UVSphere : public Model{
private:
    static int instance;
    void inline pushIndices(int ind_1, int ind_2, int ind_3);
public:
    UVSphere(float _radius, int _nCols, int _nRows);
    std::string getName(){return m.name;}
};


class Plane : public Model{
private:
    static int instance;
public: 
    Plane(glm::vec2 _size, int _ncol, int _nrows);
    std::string getName(){return m.name;}
};

#endif