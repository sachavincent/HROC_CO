#version 440 core

// BUMP MAP TESSELLATION SHADER

// number of control pts
layout (vertices = 3) out;

uniform vec3 viewPos;

uniform int tes_lod0;
uniform int tes_lod1;
uniform int tes_lod2;

// from vertex shader
in vec3 WorldPos_vert_out[];
in vec2 TexCoords_vert_out[];
in vec3 Normal_vert_out[];

// to tes eval shader
out vec3 WorldPos_ctl_out[];
out vec2 TexCoords_ctl_out[];
out vec3 Normal_ctl_out[];

float getLod(float d1, float d2);

void main(){
    // pass ctl pts
    WorldPos_ctl_out[gl_InvocationID] = WorldPos_vert_out[gl_InvocationID];
    TexCoords_ctl_out[gl_InvocationID] =  TexCoords_vert_out[gl_InvocationID];
    Normal_ctl_out[gl_InvocationID] = Normal_vert_out[gl_InvocationID];
    
    // get dist camera-vertex
    float dist1 = distance(viewPos,WorldPos_ctl_out[0]);
    float dist2 = distance(viewPos,WorldPos_ctl_out[1]);
    float dist3 = distance(viewPos,WorldPos_ctl_out[2]);
    // set lod for each edge
    gl_TessLevelOuter[0] = getLod(dist2,dist3);
    gl_TessLevelOuter[1] = getLod(dist3,dist1);
    gl_TessLevelOuter[2] = getLod(dist1,dist2);
    gl_TessLevelInner[0] = gl_TessLevelOuter[2];
}

// compute tess level with edge to camera distance
float getLod(float d1, float d2)
{
    // get average dist of edge from camera
    float avg = (d1 + d2) / 2.0;

    // low distance, high lod
    if (avg <= 2.0) {
        return tes_lod0;
    }
    else if (avg <= 5.0) {
        return tes_lod1;
    }
    else {
        return tes_lod2;
    }
}