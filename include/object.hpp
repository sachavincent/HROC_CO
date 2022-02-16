#ifndef OBJECT_H
#define OBJECT_H

#define MAXLIGHTS 10

#include <vector>
#include <string>
#include <fstream>
#include <memory>
#include <limits>

#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/gtx/string_cast.hpp>

#include "shader.hpp"
#include "light.hpp"
#include "camera.hpp"
#include "texture.hpp"
#include "bvh/observable.hpp"

class Scene;
class BoundingBox;

class Object : public Observable
{
protected:
    std::string name;

    size_t id;
    static size_t id_counter;  //TODO: reset this field in scene destructor 

    // shading
    glm::vec3 diffuseColor = glm::vec3{0.9f};
    glm::vec3 specularColor = glm::vec3{1.0f};
    float shininess = 64.0f;

    Texture diffuseMap = Texture::DEFAULT_TEXTURE();
    Texture specularMap = Texture::DEFAULT_TEXTURE();
    std::string diffuseMapPath = "";
    std::string specularMapPath = "";

    glm::vec2 texScaling = {1, 1};

    glm::mat4 scale = glm::mat4{1.0};
    glm::mat4 position = glm::mat4{1.0};
    glm::mat4 rotation = glm::mat4{1.0};

    bool loaded = false;

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
        unsigned int vbo, nbo, ebo, tbo, vao, numIndices;

        std::vector<GLfloat> vertices, normals, textureCoord;
        std::vector<GLuint> indices;
    };

    OBJECT_DATA m;

protected:
    // LOCAL object maximum and minimum bounds without transformation
    struct OBJECT_BOUNDS{
        glm::vec3 max;
        glm::vec3 min;
    };
    OBJECT_BOUNDS bounds;

public:
    Object(std::string _name = "Object")
        : name(_name), boundingBox(nullptr)
    {
        id = id_counter++;
        observer = nullptr;
    }

    ~Object();
    inline size_t getId() const { return id; }

    //! Load the object on the gpu. This action is performed after opengl/glfw initialization
    virtual void load();

    //! Render the object on screen.
    void draw(Shader &_shader);

    Object &setPosition(const glm::vec3 &_pos);
    Object &setScale(const glm::vec3 &_scale);
    Object &setRotation(float _angle, glm::vec3 _axis);
    Object &setRotationMatrix(const glm::mat4 &_rotationMatrix);

    Object &setTexDiffuse(std::string _path);
    Object &setTexSpecular(std::string _path);
    Object &setTexScaling(glm::vec2 _scale);

    //! Set color of object (not used if textures are defined)
    Object &setDiffuse(glm::vec3 _color);
    Object &setSpecular(glm::vec3 _color);
    Object &setShininess(float _shininess);

    inline void setVisible(bool _visible) { visible = _visible; }
    inline void setBoundingBox(std::shared_ptr<BoundingBox> _boundingBox) { boundingBox = _boundingBox; }
    inline std::shared_ptr<BoundingBox> getBoundingBox() { return boundingBox; }
    inline std::string getName() const { return name; }

    inline const glm::vec3 getPosition() const { return glm::vec3(position[3]); }
    inline const glm::vec3 getScale() const { return glm::vec3(scale[0][0], scale[1][1], scale[2][2]); }
    inline const glm::mat4 getRotation() const { return glm::mat4(rotation); }

    //const std::vector<GLfloat> &getVertices() const { return m.vertices; }
    //! Returns the maximum and minimum bounds after transformation
    std::pair<glm::vec3,glm::vec3> getBounds() const;
    const glm::vec3 getDiffuse() const { return diffuseColor; }
    const glm::vec3 getSpecular() const { return specularColor; }
    float getShininess() const { return shininess; }

    const glm::vec2 getTexScaling() const { return texScaling; }


    void registerObserver(Observer &o) override;
    void removeObserver(Observer &o) override;
    void notifyObservers() override;

    virtual bool hasTextures() { return diffuseMap != Texture::DEFAULT_TEXTURE(); }
    //! before creating a new scene, flush the static caches of objects.
    static void flushCaches();
};

//! a simple cube to test shader on
class Cube : public Object
{
    friend class Object;
    int instance;
    

protected:
    static int instance_counter;//TODO: reset this field in scene destructor
    static unsigned int shared_vao; //TODO: reset this field in scene destructor 
public:
    //! Create a cube of size _edgeSize.
    Cube(float _edgeSize = 1.0f, std::string _name = "");
    virtual void load();
};

//! A object loaded from a file, only one object per .obj.
class FileObject : public Object
{
private:
friend class Object;
    static int instance_counter;//TODO: reset this field in scene destructor
    int instance;
    std::string abs_path;
    // map < path of object , <vao, numIndices, bounds>>
    static std::map<const std::string,
            std::tuple<uint32_t,uint32_t,Object::OBJECT_BOUNDS>> path_cache;//TODO: reset this field in scene destructor 

    void processMesh(aiMesh *_mesh, const aiScene *_scene);

public:
    FileObject(std::string _path, bool _smoothNormals, std::string _name = "");
    virtual void load();

    bool hasTextures() { return diffuseMapPath != ""; }
};

//! Load an assimp mesh as an object
class AssimpMeshObject : public Object
{
private:
friend class Object;
    static int instance_counter;//TODO: reset this field in scene destructor
    int instance;

    using Object::setPosition;// AssimpMeshObject cannot be transformed
    using Object::setScale;
    using Object::setRotation;
    using Object::setRotationMatrix;
 
public:
    AssimpMeshObject(const aiScene *_scene, aiMesh *_mesh, std::string _name = "");

    bool hasTextures() { return diffuseMapPath != ""; }
};

class UVSphere : public Object
{
    friend class Object;
private:
    static int instance;  //TODO: reset this field in scene destructor 
    void inline pushIndices(int ind_1, int ind_2, int ind_3);

public:
    UVSphere(float _radius, int _nCols, int _nRows, const std::string &_name = "UVSphere_" + std::to_string(instance));
};

class Plane : public Object
{
    friend class Object;
private:
    static int instance;  //TODO: reset this field in scene destructor 

public:
    Plane(glm::vec2 _size, int _nCols, int _nRows, const std::string &_name = "Plane_" + std::to_string(instance));
};

#endif