#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 instanceMatrix;
layout (location = 7) in uint aObjectID;
layout (location = 8) in vec3 aDiffuse;


out vec3 Normal_in;
out vec3 FragPos_in;
out vec2 TexCoords_in;
flat out uint ObjectID_in;
out vec3 Diffuse_in;

uniform mat4 view;
uniform mat4 projection;


void main(){

    FragPos_in = vec3((instanceMatrix*vec4(aPos,1.0)));
    Normal_in = mat3(transpose(inverse(instanceMatrix))) * aNormal;
    Normal_in = normalize(Normal_in);
    TexCoords_in = aTexCoords;

    gl_Position = projection * view * vec4(FragPos_in, 1.0);
    ObjectID_in = aObjectID;
    Diffuse_in = aDiffuse;
}