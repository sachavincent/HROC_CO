#ifndef MODEL_H
#define MODEL_H

#define MAXLIGHTS 100

#include "headers.hpp"
#include "shader.hpp"
#include "light.hpp"
#include "camera.hpp"
#include "utils.hpp"
#include "texture.hpp"


#include <vector>
#include <string>

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
        std::string shaderTescPath = "";
        std::string shaderTesePath = "";

        Texture diffuseMap = Texture::DEFAULT_TEXTURE();
        Texture specularMap = Texture::DEFAULT_TEXTURE();
        Texture metallicMap = Texture::DEFAULT_TEXTURE();
        Texture heightMap = Texture::DEFAULT_TEXTURE();
        Texture normalMap = Texture::DEFAULT_TEXTURE();
        Texture AOMap = Texture::DEFAULT_TEXTURE();

        std::string diffuseMapPath = "";
        std::string specularMapPath = "";
        std::string metallicMapPath = "";
        std::string heightMapPath = "";
        std::string normalMapPath = "";
        std::string AOMapPath = "";

        glm::vec3 diffuseColor = glm::vec3{0.9};
        glm::vec3 specularColor = glm::vec3{1.0};
        float metallic = 0.0f;
        float roughness = 0.0f;
        float shininess = 64.0f;

        glm::vec2 texScaling = {1,1};

        std::vector<GLfloat> vertices, normals, textureCoord;
        std::vector<GLuint>  indices;

        float displacementStrength = 0.01f;

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
    
    //! render objects faster for shadow map
    /**
     * \param _shader the depth map shader.
     **/
    virtual void renderForDepth(Shader& _shader);

    virtual Model& setScale(glm::vec3 _scale);
    virtual Model& setRotation(float _angle, glm::vec3 _axis);
    virtual Model& setPosition(glm::vec3 _pos);

    Model& setTexDiffuse(std::string _path);
    Model& setTexSpecular(std::string _path);

    Model& setTexAlbedo(std::string _path);
    Model& setTexRoughness(std::string _path);
    Model& setTexMetallic(std::string _path);

    Model& setTexHeight( std::string _path);
    Model& setTexNormal( std::string _path);
    Model& setTexAO( std::string _path);

    virtual Model& setTexScaling( glm::vec2 _scale);

    //! Set color of object (unused if textures are defined)
    virtual Model& setDiffuse(glm::vec3 _color);
    virtual Model& setSpecular(glm::vec3 _color);
    virtual Model& setAlbedo(glm::vec3 _color);
    virtual Model& setRoughness(float _roughness);
    virtual Model& setMetallic(float _metallic);
    virtual Model& setShininess(float _shininess);

    virtual glm::vec3 getPosition(){return glm::vec3(m.translate[3]);}
    virtual glm::vec3 getScale(){return glm::vec3(m.scale[0][0],m.scale[1][1],m.scale[2][2]);}
    virtual glm::vec3 getDiffuse(){return m.diffuseColor;}
    virtual glm::vec3 getSpecular(){return m.specularColor;}
    virtual float getRoughness(){return m.roughness;}
    virtual float getMetallic(){return m.metallic;}
    virtual float getShininess() {return m.shininess;}
    virtual glm::vec2 getTexScaling(){return m.texScaling;}
    virtual std::string getName() = 0;
    virtual bool hasTextures(){return m.diffuseMap != Texture::DEFAULT_TEXTURE();}


    //! Set the displacement mutiplier factor to control displacement amount
    Model& displacementStrength(float _strength);

    
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
    void renderForDepth(Shader& _shader); 

    FileModel& setScale(glm::vec3 _scale);
    FileModel& setRotation(float _angle, glm::vec3 _axis);
    FileModel& setPosition(glm::vec3 _pos);
    FileModel& setDiffuse(glm::vec3 _color);
    FileModel& setSpecular(glm::vec3 _color);
    FileModel& setRoughness(float _roughness);
    FileModel& setMetallic(float _metallic);
    FileModel& setAlbedo(glm::vec3 _color);
    FileModel& setShininess(float _shininess);

    std::string getName(){return name;}
    glm::vec3 getPosition(){return glm::vec3(subModels[0].translate[3]);}
    glm::vec3 getScale(){return glm::vec3(subModels[0].scale[0][0],
                                        subModels[0].scale[1][1]
                                        ,subModels[0].scale[2][2]);}
    glm::vec3 getDiffuse(){return subModels[0].diffuseColor;}
    glm::vec3 getSpecular(){return subModels[0].specularColor;}
    float getRoughness(){return subModels[0].roughness;}
    float getMetallic(){return subModels[0].metallic;}
    float getShininess() {return subModels[0].shininess;}
    bool hasTextures(){return false;}

};


class CubeMap : public Cube{
private:
    int cubeMap;
    std::string cubeMapDir;
    void loadShaders();
    
public:
    CubeMap(){};
    /**
    *  Create a Cubemap to be added to a scene.
    *  File extensions are found automatically (png,jpg,jpeg are supported).
    * \param _directory The texture files directory
    **/
    CubeMap(std::string _directory);

    std::string getName(){return m.name;}
    void load();
    void render(Scene* _scene);
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