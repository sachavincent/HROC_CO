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

    virtual bool hasShadowMap() = 0;
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

    bool hasShadowMap() {return false;}
    bool isDistant(){return false;}
};

class DistantLight : public Light {
   private:
    // shadow map
    bool shadowMapEnabled = false;
    uint32_t depthTexture;
    uint32_t textureRes;
    uint32_t fbo;
    float domainSize;

    glm::mat4 lightSpaceMatrix;

    void updateLightMatrix(){
        glm::mat4 projection =
                glm::ortho(-domainSize, domainSize, -domainSize, domainSize, 0.1f, domainSize*2);
        glm::mat4 view = glm::lookAt(glm::normalize(position)*(domainSize/2), glm::vec3(0.0f),
                                         glm::vec3(0.0, 1.0, 0.0));

        lightSpaceMatrix = projection * view;
    }

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

        updateLightMatrix();
    }
    
    DistantLight(const DistantLight& l) : Light(l,"DistantLight_") {}
    ~DistantLight(){};

    DistantLight& enableShadowMap(int _resolution,float _domainSize) {
        shadowMapEnabled = true;
        textureRes = _resolution;
        domainSize = _domainSize;

        updateLightMatrix();
        return *this;
    }

    DistantLight& disableShadowMap() {
        shadowMapEnabled = false;
        return *this;
    }

    

    DistantLight& updateShadowMap(int _resolution,float _domainSize,bool _shadowMapEnabled) {
        shadowMapEnabled = _shadowMapEnabled;
        textureRes = _resolution;
        domainSize = _domainSize;

        if(shadowMapEnabled){
            updateLightMatrix();
            createDepthBuffer();
        }
        return *this;
    }


    void createDepthBuffer() {
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        // create depth texture
        glGenTextures(1, &depthTexture);
        glBindTexture(GL_TEXTURE_2D, depthTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, textureRes,
                     textureRes, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture,
                             0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    uint32_t getDepthTexture() { return depthTexture; }

    bool hasShadowMap() { return shadowMapEnabled; }
    
    uint32_t getSMRes() { return textureRes; }

    float getSMDomainSize() { return domainSize; }
    
    uint32_t getFbo(){return fbo;}

    glm::mat4 getLightSpacematrix(){
        return lightSpaceMatrix;
    }

    void setPosition(glm::vec3 _pos){
        position = 10000.0f*glm::normalize(_pos);
        updateLightMatrix();
    }
    bool isDistant(){return true;}
};



#endif

