#ifndef SSAO_H
#define SSAO_H

#include<random>

#include "headers.hpp"

#include "camera.hpp"


class SSAO {
private:
    // input textures
    uint32_t positionTexture;
    uint32_t normalTexture;
    uint32_t depthTexture;

    // texture of ssao pass
    uint32_t ssaoTexture;
    uint32_t noiseTexture;
    // final texture of blur after ssao pass
    uint32_t blurTexture;

    uint32_t fboGeo;
    uint32_t fboSSAO;
    uint32_t fboBlur;

    //quad data to render ssao textures
    uint32_t quadVAO; 
    uint32_t quadVBO;

    //the noise kernel for ssao sampling
    std::vector<glm::vec3> kernel;
    std::vector<glm::vec3> noise;

    Shader shGeo;
    Shader shSSAO;
    Shader shBlur;

    glm::vec2 lastRes;

public:
    SSAO(){};
    SSAO(Camera& _cam){

        //load shaders for ssao
        shGeo = {"shaders/SSAO/SSAOgeometry.vert","shaders/SSAO/SSAOgeometry.frag"};
        shSSAO = {"shaders/SSAO/SSAO.vert","shaders/SSAO/SSAO.frag"};
        shBlur = {"shaders/SSAO/SSAO.vert","shaders/SSAO/SSAOblur.frag"};

       float quadVertices[] = {
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f
        };
    
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);

        glBindVertexArray(quadVAO);

        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0 );
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glBindVertexArray(0);

        // create the kernel samples
        std::uniform_real_distribution<float> rnd(0.0, 1.0); // random floats between [0.0, 1.0]
        std::default_random_engine gen;
        for (uint32_t i = 0; i < 64; ++i){   
            //gen point in hemisphere
            glm::vec3 point(
                rnd(gen) * 2.0 - 1.0, 
                rnd(gen) * 2.0 - 1.0, 
                rnd(gen)
            );
            point  = glm::normalize(point);

            float scale = (float)i / 64.0f;
            scale = 0.1f + (scale * scale)*0.9f;
            point *= scale;
            kernel.push_back(point);
        }
        
        // create noise texture
        for (uint32_t i = 0; i < 16; i++){
            glm::vec3 val(rnd(gen) * 2.0 - 1.0, rnd(gen) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
            noise.push_back(val);
        }

        //create our fbos
        glGenFramebuffers(1, &fboGeo);
        glBindFramebuffer(GL_FRAMEBUFFER, fboGeo);

        GLenum attachmentGeo[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
        glDrawBuffers(2, attachmentGeo);

        glGenFramebuffers(1, &fboSSAO);
        glBindFramebuffer(GL_FRAMEBUFFER, fboSSAO);

        glGenFramebuffers(1, &fboBlur);
        glBindFramebuffer(GL_FRAMEBUFFER, fboBlur);
        
        
        lastRes = {_cam.getResWidth(),_cam.getResHeight()};

        genTextures();

    }

    void genTextures(){
        int width = lastRes[0];
        int height = lastRes[1];
        // create input positions texture
        glGenTextures(1, &positionTexture);
        glBindTexture(GL_TEXTURE_2D, positionTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width,
                     height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // create input normals texture
        glGenTextures(1, &normalTexture);
        glBindTexture(GL_TEXTURE_2D, normalTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width,
                     height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // create input normals texture
        glGenTextures(1, &depthTexture);
        glBindTexture(GL_TEXTURE_2D, depthTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width,
                     height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // ssao pass texture
        glGenTextures(1, &ssaoTexture);
        glBindTexture(GL_TEXTURE_2D, ssaoTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, width,
                     height, 0, GL_RED, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        //create noise kernel texture for ssao
        glGenTextures(1, &noiseTexture);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &noise[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //repeat noise on screen
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  

        // create ssao blur pass texture
        glGenTextures(1, &blurTexture);
        glBindTexture(GL_TEXTURE_2D, blurTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, width,
                     height, 0, GL_RED, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    Shader& setupGeometryPass(Camera& _cam){

        // redefine textures if resolution changed
        if(lastRes != glm::vec2{_cam.getResWidth(),_cam.getResHeight()}){
            lastRes = {_cam.getResWidth(),_cam.getResHeight()};
            genTextures();
            std::cout << "regen textures";
        }
        
        glBindFramebuffer(GL_FRAMEBUFFER, fboGeo);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //glEnable(GL_DEPTH_TEST);
        //glDepthMask(GL_TRUE);

        shGeo.use();
        shGeo.setMat4("view", _cam.getView());
        shGeo.setMat4("projection",_cam.getProj());

        // bind it's output textures
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, positionTexture,0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalTexture,0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture ,0);

        return shGeo;
    };

    void SSAOPass(Camera& _cam){
        //bind fbo
        glBindFramebuffer(GL_FRAMEBUFFER, fboSSAO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoTexture,
                             0);

        shSSAO.use();
        shSSAO.setInt("gPosition",0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, positionTexture);

        shSSAO.setInt("gNormal",1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalTexture);

        shSSAO.setInt("texNoise",2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);

        shSSAO.setVec2("screenSize",glm::vec2{_cam.getResWidth(),_cam.getResHeight()});
        shSSAO.setMat4("projection",_cam.getProj());
        
        for (unsigned int i = 0; i < 64; ++i)
            shSSAO.setVec3("samples[" + std::to_string(i) + "]", kernel[i]);

        //draw quad
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    };

    void blurPass(){

        glBindFramebuffer(GL_FRAMEBUFFER, fboBlur);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurTexture, 0);

        shBlur.use();

        shBlur.setInt("ssaoInput",0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ssaoTexture);

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    };

    uint32_t getSSAOTexture(){return ssaoTexture;};
    uint32_t getSSAOBlurTexture(){return blurTexture;};

};

#endif