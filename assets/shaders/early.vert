#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 3) in mat4 instanceMatrix;
layout (location = 7) in uint objectID;

uniform mat4 view;
uniform mat4 projection;

flat out uint pass_objectID;

void main()
{
    vec3 FragPos_in = vec3((instanceMatrix * vec4(aPos, 1.0)));
    gl_Position = projection * view * vec4(FragPos_in, 1.0);
    pass_objectID = objectID;
}