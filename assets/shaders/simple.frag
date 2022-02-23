#version 460 core

const vec3 OCCLUDER_COLOR = vec3(0.88, 0.07, 0.11);
const vec3 OCCLUDEE_COLOR = vec3(0.17, 0.03, 0.72);

out vec4 FragColor;

in vec3 FragPos_in;
in vec2 TexCoords_in;
in vec3 Normal_in;
flat in uint ObjectID_in;
in vec3 Diffuse_in;
    
layout (binding = 0) uniform sampler2DArray textureArray;

uniform bool debugVisibility;
uniform bool visible;

void main()
{
    if(debugVisibility)
    {
        if(visible)
            FragColor = vec4(OCCLUDER_COLOR, 1.0);
        else
            FragColor = vec4(OCCLUDEE_COLOR, 1.0);
    }
    else
    {
        if(Diffuse_in.x < 0)
            FragColor = texture(textureArray, vec3(TexCoords_in.x, TexCoords_in.y, Diffuse_in.z));
        else
            FragColor = vec4(Diffuse_in, 1.0);
    }
}