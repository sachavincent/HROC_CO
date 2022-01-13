#version 440 core

// BUMP MAP TESSELLATION SHADER

layout(triangles, equal_spacing, ccw) in;

// vert disp parameters
uniform sampler2D dispMap;
uniform float dispStrength;
uniform vec2 texScaling;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix[5];

in vec3 WorldPos_ctl_out[];
in vec2 TexCoords_ctl_out[];
in vec3 Normal_ctl_out[];

out vec3 FragPos_in;
out vec2 TexCoords_in; 
out vec3 Normal_in;
out vec4 Fragpos_lightSpace_in[5];

// get interpolation between 3 vectors
vec2 interp2D(vec2 v0, vec2 v1, vec2 v2);
vec3 interp3D(vec3 v0, vec3 v1, vec3 v2);

void main(){
    // interpolate and send to frag shader
    TexCoords_in = interp2D(TexCoords_ctl_out[0], TexCoords_ctl_out[1], TexCoords_ctl_out[2]);
    Normal_in = interp3D(Normal_ctl_out[0], Normal_ctl_out[1], Normal_ctl_out[2]);
    Normal_in = normalize(Normal_in);
    FragPos_in = interp3D(WorldPos_ctl_out[0], WorldPos_ctl_out[1], WorldPos_ctl_out[2]);
    //do displacement
    float disp = texture(dispMap, TexCoords_in*texScaling).r;
    FragPos_in += Normal_in * disp * dispStrength;

    for (int i = 0; i<5;i++){
        Fragpos_lightSpace_in[i] = lightSpaceMatrix[i] * vec4(FragPos_in, 1.0);
    }

    gl_Position = projection * view * vec4(FragPos_in,1.0);
}

vec2 interp2D(vec2 v0, vec2 v1, vec2 v2){
    return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}

vec3 interp3D(vec3 v0, vec3 v1, vec3 v2){
    return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}