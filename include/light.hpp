#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
class Light
{
public:
    Light(glm::vec3 pos = {0, 0, 0}, glm::vec3 color = {0, 0, 0}, glm::vec3 attenuation = {0, 0, 1}) : _position(pos), _color(color), _attenuation(attenuation)
    {
    }

    inline const glm::vec3 &getPosition() const { return _position; }

    inline const glm::vec3 &getColor() const { return _color; }

    inline const glm::vec3 &getAttenuation() const { return _attenuation; }

    void setPosition(const glm::vec3 pos) { _position = pos; }

    void setColor(const glm::vec3 color) { _color = color; }

    void setAttenuation(const glm::vec3 attenuation) { _attenuation = attenuation; }

private:
    glm::vec3 _position;
    glm::vec3 _color;
    glm::vec3 _attenuation;
};
#endif
