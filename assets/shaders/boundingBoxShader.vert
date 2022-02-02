#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;


out vec3 Normal_in;
out vec3 FragPos_in;
out vec2 TexCoords_in;
out int Depth_in;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform int depth;


void main()
{
    FragPos_in = vec3((model*vec4(aPos,1.0)));
    Normal_in = mat3(transpose(inverse(model))) * aNormal;
    Normal_in = normalize(Normal_in);
    TexCoords_in = aTexCoords;

    gl_Position = projection * view * vec4(FragPos_in, 1.0);
    Depth_in = depth;
}

