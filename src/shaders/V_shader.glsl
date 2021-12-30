#version 430 core

layout (location = 0) in vec3 position; 
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

out vec2 pass_uv;
out vec3 pass_normal;

uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;

void main()
{
    gl_Position = view_matrix * projection_matrix * model_matrix * pos;
    pass_normal = normal;
    pass_uv = uv;
}