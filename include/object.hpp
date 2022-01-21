#ifndef OBJECT_HPP
#define OBJECT_HPP
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "texture.hpp"
#include "observable.hpp"

class BoundingBox;
class Object : public Observable
{
    Object(glm::vec3 pos, int vao, int vbo, int ebo);
    Object(glm::vec3 pos, const std::string &file);

    ~Object() override;

    inline const long &getId() const { return _id; }
    inline const glm::vec3 &getPosition() const { return glm::vec3(_transform[3][0], _transform[3][1], _transform[3][2]); }
    inline const glm::mat3 &getRotation() const { return glm::mat3(); } //TODO: https://math.stackexchange.com/questions/237369/given-this-transformation-matrix-how-do-i-decompose-it-into-translation-rotati/3554913
    inline const glm::mat4 &getTransform() const { return _transform; }
    inline const std::vector<Texture> &getTextures() const { return _textures; }

    inline void setId(long id) { _id = id; }
    inline void setVisible(bool visible) { _visible = visible; }
    inline void setBoundingBox(BoundingBox *boundingBox) { _boundingBox = boundingBox; }

    void registerObserver(Observer &o) override;
    void removeObserver(Observer &o) override;
    void notifyObservers() override;

    void draw();

private:
    long _id;
    int _vao;
    int _vbo;
    int _ebo;
    glm::mat4 _transform;

    bool _visible;

    std::vector<Texture> _textures;

    BoundingBox *_boundingBox;

    Observer *_observer;
};
#endif