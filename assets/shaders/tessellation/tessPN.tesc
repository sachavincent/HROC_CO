#version 460 core

// PN TRIANGLES TESSELLATION SHADER

// number of control pts
layout (vertices = 1) out;

uniform int tes_lod0;
uniform int tes_lod1;
uniform int tes_lod2;

uniform vec3 viewPos;

// from vertex shader
in vec3 WorldPos_vert_out[];
in vec2 TexCoords_vert_out[];
in vec3 Normal_vert_out[];

struct CtlTriangle{
    vec3 p_003, p_012,p_021,p_030,
            p_102,p_111,p_120,
               p_201,p_210,
                  p_300;
    vec3 Normal[3];
    vec2 TexCoords[3];
};
//send control triangle to eval shader
patch out CtlTriangle CtlTri;

vec3 orthoProj(vec3 orig, vec3 target, vec3 normal);
float getLod(float d1, float d2, vec3 edge);

void main(){
    // pass original points
    for (int i = 0 ; i < 3 ; i++) {
       CtlTri.Normal[i] = Normal_vert_out[i];
       CtlTri.TexCoords[i] = TexCoords_vert_out[i];
    }

    //1. original points of triangle
    CtlTri.p_030 = WorldPos_vert_out[0];
    CtlTri.p_003 = WorldPos_vert_out[1];
    CtlTri.p_300 = WorldPos_vert_out[2];

    vec3 edge1 = CtlTri.p_300 - CtlTri.p_003;
    vec3 edge2 = CtlTri.p_030 - CtlTri.p_300;
    vec3 edge3 = CtlTri.p_003 - CtlTri.p_030;

    //calc tessellation levels with edges
    float dist1 = distance(viewPos,WorldPos_vert_out[0]);
    float dist2 = distance(viewPos,WorldPos_vert_out[1]);
    float dist3 = distance(viewPos,WorldPos_vert_out[2]);

    gl_TessLevelOuter[0] = getLod(dist2,dist3,edge1);
    gl_TessLevelOuter[1] = getLod(dist3,dist1,edge2);
    gl_TessLevelOuter[2] = getLod(dist1,dist2,edge3);
    gl_TessLevelInner[0] = gl_TessLevelOuter[2];

    //2. points on edges
    CtlTri.p_012 = CtlTri.p_030 + edge3 * 2.0 / 3.0; 
    CtlTri.p_021 = CtlTri.p_030 + edge3 / 3.0;       

    CtlTri.p_201 = CtlTri.p_003 + edge1 * 2.0 / 3.0;
    CtlTri.p_102 = CtlTri.p_003 + edge1 / 3.0;

    CtlTri.p_120 = CtlTri.p_300 + edge2 * 2.0 / 3.0;
    CtlTri.p_210 = CtlTri.p_300 + edge2 / 3.0;
    // project edge points
    CtlTri.p_021 = orthoProj(CtlTri.p_021, CtlTri.p_030,CtlTri.Normal[0]);
    CtlTri.p_012 = orthoProj(CtlTri.p_012, CtlTri.p_003,CtlTri.Normal[1]);
    CtlTri.p_102 = orthoProj(CtlTri.p_102, CtlTri.p_003,CtlTri.Normal[1]);
    CtlTri.p_201 = orthoProj(CtlTri.p_201, CtlTri.p_300,CtlTri.Normal[2]);
    CtlTri.p_210 = orthoProj(CtlTri.p_210, CtlTri.p_300,CtlTri.Normal[2]);
    CtlTri.p_120 = orthoProj(CtlTri.p_120, CtlTri.p_030,CtlTri.Normal[0]);


    //3. center of triangle
    vec3 triCenter = (CtlTri.p_003 + CtlTri.p_030 + CtlTri.p_300) / 3.0;

     CtlTri.p_111 = (CtlTri.p_021 + CtlTri.p_012 + CtlTri.p_102 +
                          CtlTri.p_201 + CtlTri.p_210 + CtlTri.p_120) / 6.0;
    
    CtlTri.p_111 += (CtlTri.p_111 - triCenter) / 2.0;
}

vec3 orthoProj(vec3 orig, vec3 target, vec3 normal){
    vec3 v = orig - target;
    float Len = dot(v, normal);
    vec3 d = Len * normal;
    return (orig - d);
}

// compute tess level with edge to camera distance
float getLod(float d1, float d2,vec3 edge)
{
    // get average dist of edge from camera
    float avg = (d1 + d2) / 2.0;

    // low distance, high lod 
    if (avg <= 2.0) {
        return tes_lod0*length(edge);
    }
    else if (avg <= 5.0) {
        return tes_lod1*length(edge);
    }
    else {
        return tes_lod2*length(edge);
    }
}