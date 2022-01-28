#include "object.hpp"

#include <algorithm>

#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"
#include "scene.hpp"

size_t Object::id_counter = 0;

void Object::load() {
    // gen geometry buffers
    glGenBuffers(1, &m.vbo);
    glGenBuffers(1, &m.nbo);
    glGenBuffers(1, &m.tbo);
    glGenBuffers(1, &m.ebo);
    glGenVertexArrays(1, &m.vao);

    // Bind the vao
    glBindVertexArray(m.vao);

    // create and fill vertex data
    glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
    glBufferData(GL_ARRAY_BUFFER, m.vertices.size() * sizeof(GLfloat),
                 m.vertices.data(), GL_STATIC_DRAW);
    // set vertex attribute pointer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // copy normals to nbo
    glBindBuffer(GL_ARRAY_BUFFER, m.nbo);
    glBufferData(GL_ARRAY_BUFFER, m.normals.size() * sizeof(GLfloat),
                 m.normals.data(), GL_STATIC_DRAW);
    // define array for normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glEnableVertexAttribArray(1);

    // Copy texture array in element buffer
    glBindBuffer(GL_ARRAY_BUFFER, m.tbo);
    glBufferData(GL_ARRAY_BUFFER, m.textureCoord.size() * sizeof(GLfloat),
                 m.textureCoord.data(), GL_STATIC_DRAW);
    // define array for texture
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glEnableVertexAttribArray(2);

    // copy indices to ebo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m.indices.size() * sizeof(GLfloat),
                 m.indices.data(), GL_STATIC_DRAW);

    // Unbind the VAO
    glBindVertexArray(0);

    // load textures if defined
    if (diffuseMapPath != "") {
        diffuseMap = Texture::loadTexture(diffuseMapPath.c_str());
    }
    if (specularMapPath != "") {
        specularMap = Texture::loadTexture(specularMapPath.c_str());
    }
    loadShaders();
}

void Object::draw(Scene* _scene) {
    Camera& cam = _scene->getCamera();
    // std::cout << m.indices.size() << std::endl;
    auto& lights = _scene->getLights();

    shader.start();

    shader.loadMat4("model", translate * rotation * scale);
    shader.loadMat4("view", cam.getViewMatrix());
    shader.loadMat4("projection", cam.getProjectionMatrix());
    shader.loadVec2("screenSize",
                    glm::vec2(cam.getResWidth(), cam.getResHeight()));
    shader.loadVec3("viewPos", cam.getPosition());
    shader.loadFloat("exposure", _scene->getExposure());
    shader.loadVec2("texScaling", texScaling);

    shader.loadFloat("material.specularStrength", 0.5f);
    shader.loadFloat("material.shininess", shininess);

    // bind diffuse/albedo texture
    glActiveTexture(GL_TEXTURE0);
    if (diffuseMap != Texture::DEFAULT_TEXTURE()) {
        shader.loadInt("material.diffuseTex", 0);
        shader.loadBool("material.hasTexture", true);
        diffuseMap.load();
    } else {
        shader.loadVec3("material.diffuse", diffuseColor);
        shader.loadBool("material.hasTexture", false);
        diffuseMap.unload();
    }
    // bind specular/roughness texture
    glActiveTexture(GL_TEXTURE1);
    if (specularMap != Texture::DEFAULT_TEXTURE()) {
        shader.loadInt("material.specularTex", 1);
        specularMap.load();
    } else {
        shader.loadVec3("material.specular", specularColor);
        specularMap.unload();
    }
    // bind mellatlic texture (if PBR)
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);

    for (uint32_t i = 0; i < std::min(lights.size(), (size_t)MAXLIGHTS); i++) {
        shader.loadBool("lights[" + std::to_string(i) + "].enabled", 1);

        shader.loadVec3("lights[" + std::to_string(i) + "].position",
                        lights[i]->getPosition());

        shader.loadVec3("lights[" + std::to_string(i) + "].color",
                        lights[i]->getColor());
        shader.loadVec3("lights[" + std::to_string(i) + "].attenuation",
                        lights[i]->getAttenuation());
    }
    if (lights.size() < MAXLIGHTS) {
        for (size_t i = lights.size(); i < MAXLIGHTS; i++) {
            shader.loadBool("lights[" + std::to_string(i) + "].enabled", 0);
        }
    }

    glBindVertexArray(m.vao);

    glDrawElements(GL_TRIANGLES, m.indices.size(), GL_UNSIGNED_INT, nullptr);

    glBindVertexArray(0);
    shader.stop();
}

void Object::loadShaders() {
    shader = {"shaders/default.vert", "shaders/phong.frag"};
}

Object& Object::setScale(glm::vec3 _scale) {
    scale = glm::mat4(1.0);
    scale = glm::scale(scale, _scale);
    return *this;
}

Object& Object::setRotation(float _angle, glm::vec3 _axis) {
    rotation = glm::mat4(1.0);
    rotation = glm::rotate(rotation, glm::radians(_angle), _axis);
    return *this;
}

Object& Object::setPosition(glm::vec3 _pos) {
    translate = glm::mat4{1.0};
    translate = glm::translate(translate, _pos);
    return *this;
}

Object& Object::setTexDiffuse(std::string _path) {
    diffuseMapPath = _path;
    return *this;
}

Object& Object::setTexSpecular(std::string _path) {
    specularMapPath = _path;
    return *this;
}

Object& Object::setShininess(float _shininess) {
    shininess = _shininess;
    return *this;
}

Object& Object::setDiffuse(glm::vec3 _color) {
    diffuseColor = _color;
    return *this;
}

Object& Object::setSpecular(glm::vec3 _color) {
    specularColor = _color;
    return *this;
}

Object& Object::setTexScaling(glm::vec2 _scale) {
    texScaling = _scale;
    return *this;
}

void Object::registerObserver(Observer& o) { _observer = &o; }

void Object::removeObserver(Observer& o) { _observer = nullptr; }

void Object::notifyObservers() { _observer->update(this, _visible); }
