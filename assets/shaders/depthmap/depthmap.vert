#version 440 core

layout(location = 0) in vec3 aPos;


uniform mat4 vp;
uniform mat4 model;

void main(){
    gl_Position = vp * model * vec4(aPos, 1.0);
}