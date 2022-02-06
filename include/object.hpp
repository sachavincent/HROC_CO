#ifndef OBJECT_H
#define OBJECT_H

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
#include "observable.hpp"

typedef enum
{
    SMOOTH_NORMAL_ENABLE = true,
    SMOOTH_NORMAL_DISABLE = false
} SMOOTH_NORMAL;

class Scene;
class BoundingBox;

class Object : public Observable
{
protected:
    std::string name;

    size_t id;
    static size_t id_counter;

    // shading
    glm::vec3 diffuseColor = glm::vec3{0.9f};
    glm::vec3 specularColor = glm::vec3{1.0f};
    float shininess = 64.0f;

    Texture diffuseMap = Texture::DEFAULT_TEXTURE();
    Texture specularMap = Texture::DEFAULT_TEXTURE();
    std::string diffuseMapPath = "";
    std::string specularMapPath = "";

    glm::mat4 scale, rotation, translate;
    glm::vec2 texScaling = {1, 1};

    // HROC
    std::shared_ptr<BoundingBox> boundingBox;
    Observer *observer;
    bool visible;

    //! une structure qui encapsule la description d'un seul object
    struct OBJECT_DATA
    {
        // vbo = vertices
        // nbo = normals
        // ebo = indices
        // tbo = texture coord
        unsigned int vbo, nbo, ebo, tbo, vao;

        std::vector<GLfloat> vertices, normals, textureCoord;
        std::vector<GLuint> indices;
    };

    OBJECT_DATA m;

public:
    Object(std::string _name = "Object") : name(_name), boundingBox(nullptr) { id = id_counter++; }

    inline size_t getId() const { return id; }

    //! Load the object on the gpu. This action is performed after opengl/glfw initialization
    virtual void load();

    //! Render the object on screen.
    virtual void draw(Scene *_scene);

    virtual Object &setPosition(glm::vec3 _pos);
    virtual Object &setScale(glm::vec3 _scale);
    virtual Object &setRotation(float _angle, glm::vec3 _axis);

    Object &setTexDiffuse(std::string _path);
    Object &setTexSpecular(std::string _path);
    Object &setTexScaling(glm::vec2 _scale);

    //! Set color of object (not used if textures are defined)
    virtual Object &setDiffuse(glm::vec3 _color);
    virtual Object &setSpecular(glm::vec3 _color);
    virtual Object &setShininess(float _shininess);

    inline void setVisible(bool _visible) { visible = _visible; }
    inline void setBoundingBox(std::shared_ptr<BoundingBox> _boundingBox) { boundingBox = _boundingBox; }
    inline std::shared_ptr<BoundingBox> getBoundingBox() { return boundingBox; }
    inline std::string getName() const { return name; }

    virtual glm::vec3 getPosition() { return glm::vec3(translate[3]); }
    virtual glm::vec3 getScale() { return glm::vec3(scale[0][0], scale[1][1], scale[2][2]); }

    virtual std::vector<GLfloat> &getVertices() { return m.vertices; }
    virtual glm::vec3 getDiffuse() { return diffuseColor; }
    virtual glm::vec3 getSpecular() { return specularColor; }
    virtual float getShininess() { return shininess; }

    virtual glm::vec2 getTexScaling() { return texScaling; }

    void registerObserver(Observer &o) override;
    void removeObserver(Observer &o) override;
    void notifyObservers() override;

    virtual bool hasTextures() { return diffuseMap != Texture::DEFAULT_TEXTURE(); }
};

//! a simple cube to test shader on
class Cube : public Object
{
    static int instance;

public:
    //! Create a cube of size _edgeSize.
    Cube(float _edgeSize = 1.0f, std::string _name = "Cube_" + std::to_string(instance));
};

class BoundingBoxObject : public Cube
{
private:
    glm::vec3 pos;
public:
    BoundingBoxObject(std::string _parentName, glm::vec3 center, glm::vec3 size, float _edgeSize = 1.0f);

    void draw(Scene *_scene, int _depth);
};

//! A object loaded from a file, it can contain multiple subobjects inside it.
class FileObject : public Object
{
private:
    static int instance;
    std::vector<OBJECT_DATA> subObjects;

    void processMesh(aiMesh *_mesh, const aiScene *_scene, size_t _meshIdx);

public:
    FileObject(std::string _path, SMOOTH_NORMAL _smoothNormals);
    void load();
    void draw(Scene *_scene);

    bool hasTextures() { return false; }
};

class UVSphere : public Object
{
private:
    static int instance;
    void inline pushIndices(int ind_1, int ind_2, int ind_3);

public:
    UVSphere(float _radius, int _nCols, int _nRows);
};

class Plane : public Object
{
private:
    static int instance;

public:
    Plane(glm::vec2 _size, int _ncol, int _nrows);
};

#endif