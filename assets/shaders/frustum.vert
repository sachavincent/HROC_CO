#version 460 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec3 FragPos_in = vec3((model * vec4(aPos, 1.0)));
    gl_Position = projection * view * vec4(FragPos_in, 1.0);
}