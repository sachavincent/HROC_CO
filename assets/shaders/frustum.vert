#version 460 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 staticInverse;

void main()
{

    /*
    // test
    vec4 v = inv*vec4(aPos,1.0));
    v.z = -v.z;
    vec3 worldVertice = vec3(v) / v.w;
    worldVertice.z = -worldVertice.z;
    vec3 FragPos_in = vec3((model * vec4(worldVertice, 1.0)));
    

    // gppu version
    vec3 FragPos_in = vec3((model * vec4(aPos, 1.0)));
    
    gl_Position = projection * view * vec4(FragPos_in, 1.0);
    */


    // cpu version
    vec3 FragPos_in = aPos;
    gl_Position = vec4(aPos,1.0);
}