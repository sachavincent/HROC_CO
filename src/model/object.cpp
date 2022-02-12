#include "object.hpp"

#include <algorithm>

#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"
#include "scene.hpp"

size_t Object::id_counter = 0;

Object::~Object()
{
#ifndef HROC_TESTS
    glDeleteBuffers(1, &m.vbo);
    glDeleteBuffers(1, &m.nbo);
    glDeleteBuffers(1, &m.ebo);
    if (!m.textureCoord.empty())
        glDeleteBuffers(1, &m.tbo);
    glDeleteVertexArrays(1, &m.vao);
#endif
    observer = nullptr;

    if (boundingBox)
        boundingBox.reset();
}

void Object::load()
{
#ifndef HROC_TESTS
    if(!loaded){
        // gen geometry buffers
        glGenBuffers(1, &m.vbo);
        glGenBuffers(1, &m.nbo);
        if (!m.textureCoord.empty())
            glGenBuffers(1, &m.tbo);
        glGenBuffers(1, &m.ebo);
        glGenVertexArrays(1, &m.vao);
        std::cout << "\tLoaded object " << name << " vbo=" << m.vbo << std::endl;

        // Bind the vao
        glBindVertexArray(m.vao);

        // create and fill vertex data
        glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
        glBufferData(GL_ARRAY_BUFFER, m.vertices.size() * sizeof(GLfloat), m.vertices.data(), GL_STATIC_DRAW);
        // set vertex attribute pointer
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
        glEnableVertexAttribArray(0);

        // copy normals to nbo
        glBindBuffer(GL_ARRAY_BUFFER, m.nbo);
        glBufferData(GL_ARRAY_BUFFER, m.normals.size() * sizeof(GLfloat), m.normals.data(), GL_STATIC_DRAW);
        // define array for normals
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
        glEnableVertexAttribArray(1);

    if (!m.textureCoord.empty())
    {
        // Copy texture array in element buffer
        glBindBuffer(GL_ARRAY_BUFFER, m.tbo);
        glBufferData(GL_ARRAY_BUFFER, m.textureCoord.size() * sizeof(GLfloat), m.textureCoord.data(), GL_STATIC_DRAW);
    }

    // define array for texture
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
    glEnableVertexAttribArray(2);

        // copy indices to ebo
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m.indices.size() * sizeof(GLfloat), m.indices.data(), GL_STATIC_DRAW);

        // Unbind the VAO
        glBindVertexArray(0);
    }// fin du if
    // load textures if defined
    if (diffuseMapPath != "")
    {
        diffuseMap = Texture::loadTexture(diffuseMapPath.c_str());
    }
    if (specularMapPath != "")
    {
        specularMap = Texture::loadTexture(specularMapPath.c_str());
    }
    loaded = true;
#endif
}

void Object::draw(Scene *_scene)
{
    if (!loaded)
        return;
    Shader &sh = _scene->getShader();

    sh.loadMat4("model", transformationMatrix);
    sh.loadVec2("texScaling", texScaling);
    sh.loadFloat("material.specularStrength", 0.5f);
    sh.loadFloat("material.shininess", shininess);

    // bind diffuse texture
    glActiveTexture(GL_TEXTURE0);
    if (diffuseMap != Texture::DEFAULT_TEXTURE())
    {
        sh.loadInt("material.diffuseTex", 0);
        sh.loadBool("material.hasTexture", true);
        diffuseMap.load();
    }
    else
    {
        sh.loadVec3("material.diffuse", diffuseColor);
        sh.loadBool("material.hasTexture", false);
        diffuseMap.unload();
    }
    // bind specular texture
    glActiveTexture(GL_TEXTURE1);
    if (specularMap != Texture::DEFAULT_TEXTURE())
    {
        sh.loadInt("material.specularTex", 1);
        specularMap.load();
    }
    else
    {
        sh.loadVec3("material.specular", specularColor);
        specularMap.unload();
    }

    glBindVertexArray(m.vao);

    glDrawElements(GL_TRIANGLES, m.indices.size(), GL_UNSIGNED_INT, nullptr);

    glBindVertexArray(0);
}

Object &Object::setScale(const glm::vec3 &_scale)
{
    scale = _scale;
    updateTransformationMatrix();
    return *this;
}

Object &Object::setRotation(float _angle, glm::vec3 _axis)
{
    rotationMatrix = glm::mat4(1.0);
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(_angle), _axis);
    updateTransformationMatrix();
    return *this;
}

Object &Object::setRotationMatrix(const glm::mat4 &_rotationMatrix)
{
    rotationMatrix = _rotationMatrix;
    updateTransformationMatrix();
    return *this;
}

Object &Object::setPosition(const glm::vec3 &_pos)
{
    position = _pos;
    updateTransformationMatrix();
    return *this;
}

Object &Object::setTexDiffuse(std::string _path)
{
    diffuseMapPath = _path;
    return *this;
}

Object &Object::setTexSpecular(std::string _path)
{
    specularMapPath = _path;
    return *this;
}

Object &Object::setShininess(float _shininess)
{
    shininess = _shininess;
    return *this;
}

Object &Object::setDiffuse(glm::vec3 _color)
{
    diffuseColor = _color;
    return *this;
}

Object &Object::setSpecular(glm::vec3 _color)
{
    specularColor = _color;
    return *this;
}

Object &Object::setTexScaling(glm::vec2 _scale)
{
    texScaling = _scale;
    return *this;
}

void Object::registerObserver(Observer &o) { observer = &o; }

void Object::removeObserver(Observer &o) { observer = nullptr; }

void Object::notifyObservers() { observer->update(this, visible); }
