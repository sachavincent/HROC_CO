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
uniform float exposure;

void main()
{
    vec3 color;
    if(debugVisibility)
    {
        if(visible)
            color = OCCLUDER_COLOR;
        else
            color = OCCLUDEE_COLOR;
    }
    else
    {
        if(Diffuse_in.x < 0)
            color = texture(textureArray, vec3(TexCoords_in.x, TexCoords_in.y, Diffuse_in.z)).rgb;
        else
            color = Diffuse_in;
    }
    color = vec3(1.0) - exp(-color * exposure);

    FragColor = vec4(color, 1.0);
}