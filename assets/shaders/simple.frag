#version 460 core

out vec4 FragColor;

in vec3 FragPos_in;
in vec2 TexCoords_in;
in vec3 Normal_in;
//flat in uint ObjectID_in;
in vec3 Diffuse_in;
    
layout (binding = 0) uniform sampler2DArray textureArray;


void main()
{
    if(Diffuse_in.x < 0)
        FragColor = texture(textureArray, vec3(TexCoords_in.x, TexCoords_in.y, Diffuse_in.z));
    else
        FragColor = vec4(Diffuse_in, 1.0);
}