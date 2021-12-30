#version 410 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;


uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

out vec3 fragPos;
out vec3 normal;

void main(){
    vec4 viewpos = view * model * vec4(aPos,1.0);
    fragPos = viewpos.xyz;
    normal = transpose(inverse(mat3(view * model))) * aNormal;

    gl_Position = projection * viewpos;
}