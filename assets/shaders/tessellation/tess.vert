#version 440 core

// BUMP MAP TESSELLATION SHADER

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

// to tes control shader
out vec3 WorldPos_vert_out;
out vec3 Normal_vert_out;
out vec2 TexCoords_vert_out;

uniform mat4 model;


void main(){
    WorldPos_vert_out = vec3((model*vec4(aPos,1.0)));
    Normal_vert_out = normalize(vec3((model*vec4(aNormal,0.0))));
    TexCoords_vert_out = aTexCoords;
}