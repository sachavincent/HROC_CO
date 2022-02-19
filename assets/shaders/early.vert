#version 460 core

//layout (location = 3) in int objectID;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

flat out int pass_objectID;

void main()
{
    gl_Position = projection * view * vec4(FragPos_in, 1.0);
    pass_objectID = gl_InstanceID;
}