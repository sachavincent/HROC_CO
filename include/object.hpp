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

#include "utils/utils.hpp"
#include "shader.hpp"
#include "light.hpp"
#include "camera.hpp"
#include "texture.hpp"

class Scene;
class BoundingBox;

class Object
{
protected:
    std::string name;

    size_t id;
    static size_t id_counter; // TODO: reset this field in scene destructor

    glm::mat4 scale = glm::mat4{1.0};
    glm::mat4 position = glm::mat4{1.0};
    glm::mat4 rotation = glm::mat4{1.0};

    glm::vec3 diffuse = glm::vec3(-1.0f);

    // HROC
    std::shared_ptr<BoundingBox> boundingBox;
public:
    Object(std::string _name = "Object")
        : name(_name), boundingBox(nullptr)
    {
        id = id_counter++;
    }

    ~Object();
    inline size_t getId() const { return id; }

    Object &setPosition(const glm::vec3 &_pos);
    Object &setScale(const glm::vec3 &_scale);
    Object &setRotation(float _angle, glm::vec3 _axis);
    Object &setRotationMatrix(const glm::mat4 &_rotationMatrix);

    Object &setTexDiffuse(std::string _path);
    Object &setTexSpecular(std::string _path);

    //! Set color of object (not used if textures are defined)
    Object &setDiffuse(glm::vec3 _color);
    Object &setSpecular(glm::vec3 _color);

    virtual bool isVisible() { return false; };
    virtual void setVisible(bool _visible){};
    virtual OBJECT_BOUNDS getObjectBounds() const { return {glm::vec3(), glm::vec3()}; }

    inline void setBoundingBox(std::shared_ptr<BoundingBox> _boundingBox) { boundingBox = _boundingBox; }
    inline std::shared_ptr<BoundingBox> getBoundingBox() { return boundingBox; }
    inline std::string getName() const { return name; }

    inline const glm::vec3 &getDiffuse() const { return diffuse; }
    inline const glm::vec3 getPosition() const { return glm::vec3(position[3]); }
    inline const glm::vec3 getScale() const { return glm::vec3(scale[0][0], scale[1][1], scale[2][2]); }
    inline const glm::mat4 getRotation() const { return glm::mat4(rotation); }

    glm::mat4 getTransformationMatrix() const { return position * rotation * scale; }
    //! Returns the maximum and minimum bounds after transformation
    virtual std::pair<glm::vec3, glm::vec3> getBounds() const;

    //! before creating a new scene, flush the static caches of objects.
    static void flushCaches();
};

//! a simple cube to test shader on
class Cube : public Object
{
    friend class Object;

private:
    static bool *visible;

    static OBJECT_DATA data;

    unsigned int instance;

protected:
    static unsigned int instance_counter; // TODO: reset this field in scene destructor
public:
    static void create(int nbInstances);

    static OBJECT_DATA getData();
    //! Create a cube of size _edgeSize.
    Cube(float _edgeSize = 1.0f, const std::string &_name = "Cube_" + std::to_string(instance_counter));

    inline void setVisible(bool _visible) { visible[instance] = _visible; }
    inline bool isVisible() { return visible[instance]; }
    inline OBJECT_BOUNDS getObjectBounds() const { return data.bounds; }
};

//! A object loaded from a file, only one object per .obj.
class FileObject : public Object
{
    friend class Object;

private:
    static std::map<const std::string, bool *> visible;

    unsigned int instance;
    std::string path;

private:
    static std::map<const std::string, OBJECT_DATA> path_cache; // TODO: reset this field in scene destructor

    static OBJECT_DATA processMesh(aiMesh *_mesh, const aiScene *_scene);

protected:
    static unsigned int instance_counter; // TODO: reset this field in scene destructor
public:
    static void create(const std::string &_path, int nbInstances);

    static std::vector<OBJECT_DATA> getData();

    FileObject(const std::string &_path);

    // bool hasTextures() { return diffuseMapPath != ""; }
    inline void setVisible(bool _visible) { visible[path][instance] = _visible; }
    inline bool isVisible() { return visible[path][instance]; }
    inline OBJECT_BOUNDS getObjectBounds() const { return path_cache[path].bounds; }
};
/*
//! Load an assimp mesh as an object
class AssimpMeshObject : public Object
{
private:
    friend class Object;
    static int instance_counter; // TODO: reset this field in scene destructor
    int instance;

    using Object::setPosition; // AssimpMeshObject cannot be transformed
    using Object::setRotation;
    using Object::setRotationMatrix;
    using Object::setScale;

public:
    AssimpMeshObject(const aiScene *_scene, aiMesh *_mesh, std::string _name = "");

    bool hasTextures() { return diffuseMapPath != ""; }
};
*/
class UVSphere : public Object
{
    friend class Object;

private:
    static bool *visible;

    static OBJECT_DATA data;

    unsigned int instance; // TODO: reset this field in scene destructor

protected:
    static unsigned int instance_counter; // TODO: reset this field in scene destructor
public:
    static void create(int nbInstances);

    static OBJECT_DATA getData();

    UVSphere(float _radius, int _nCols, int _nRows, const std::string &_name = "UVSphere_" + std::to_string(instance_counter));

    inline void setVisible(bool _visible) { visible[instance] = _visible; }
    inline bool isVisible() { return visible[instance]; }
    inline OBJECT_BOUNDS getObjectBounds() const { return data.bounds; }
};

class Plane : public Object
{
    friend class Object;

private:
    static bool *visible;

    static OBJECT_DATA data;

    unsigned int instance; // TODO: reset this field in scene destructor

    glm::vec2 size;
    int nX, nY;

protected:
    static unsigned int instance_counter; // TODO: reset this field in scene destructor
public:
    static void create(int nbInstances);

    static OBJECT_DATA getData();

    Plane(glm::vec2 _size, int _nCols, int _nRows, const std::string &_name = "Plane_" + std::to_string(instance_counter));

    inline void setVisible(bool _visible) { visible[instance] = _visible; }
    inline bool isVisible() { return visible[instance]; }
    inline OBJECT_BOUNDS getObjectBounds() const { return data.bounds; }
};

#endif