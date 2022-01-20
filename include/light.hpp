#ifndef LIGHT_H
#define LIGHT_H

#include "camera.hpp"
#include "headers.hpp"
#include "shader.hpp"

inline static int light_instance = 0;

class Light {
   protected:
    glm::vec3 position;

    std::string name;
    int instance;

    glm::vec3 diffuse;
    glm::vec3 specular;

    // falloff parameters
    float ambiantStrength;
    float constant;
    float linear;
    float quadratic;

   public:
    Light(){instance = light_instance++;}
    ~Light(){};
    Light(const Light& _light, std::string _name){
        position = _light.position;

        instance = light_instance++;
        name =  _name+std::to_string(instance) ;

        diffuse = _light.diffuse;
        specular = _light.specular;

        // falloff parameters
        ambiantStrength = _light.ambiantStrength;
        constant = _light.constant;
        linear = _light.linear;
        quadratic = _light.quadratic;

    }

    virtual void setPosition(glm::vec3 _pos) = 0;
    void setDiffuse(glm::vec3 _color) { diffuse = _color; }
    void setAmbiantStrength(float _strength) { ambiantStrength = _strength; }
    void setSpecular(glm::vec3 _color) { specular = _color; }
    void setConstant(float _val) { constant = _val; }
    void setLinear(float _val) { linear = _val; }
    void setQuadratic(float _val) { quadratic = _val; }

    glm::vec3 getPos() { return position; }
    glm::vec3 getAmbiant() { return diffuse * glm::vec3(ambiantStrength); }
    glm::vec3 getDiffuse() { return diffuse; }
    glm::vec3 getSpecular() { return specular; }

    float getConstant() { return constant; }
    float getLinear() { return linear; }
    float getQuadratic() { return quadratic; }
    std::string getName(){return name;}

    virtual bool isDistant() = 0;
};


class PointLight : public Light {
   public:
    PointLight(glm::vec3 _position, glm::vec3 _color) {
        name = "PointLight_"+std::to_string(instance);

        position = _position;

        diffuse = _color;
        ambiantStrength = 0.25f;
        specular = glm::vec3(1.0f, 1.0f, 1.0f);

        constant = 1.0f;
        linear = 0.22f;
        quadratic = 0.2f;
    }

    PointLight(const PointLight& l) : Light(l,"PointLight_") {}

    ~PointLight(){};

    void setPosition(glm::vec3 _pos){
        position = _pos;
    }

    bool isDistant(){return false;}
};

class DistantLight : public Light {
private:
    glm::mat4 lightSpaceMatrix;

public:
    DistantLight(glm::vec3 _position, glm::vec3 _color) {
        name = "DistantLight_"+std::to_string(instance);
        position = 10000.0f*glm::normalize(_position);

        diffuse = _color;
        ambiantStrength = 0.25f;
        specular = glm::vec3(1.0f, 1.0f, 1.0f);

        constant = -1.0f;
        linear = -1.0f;
        quadratic = -1.0f;

    }
    
    DistantLight(const DistantLight& l) : Light(l,"DistantLight_") {}
    ~DistantLight(){};

    void setPosition(glm::vec3 _pos){
        position = 10000.0f*glm::normalize(_pos);
    }

    bool isDistant(){return true;}
};



#endif

