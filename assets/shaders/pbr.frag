#version 410 core

#define NR_LIGHTS 100
#define gamma 2.2

// PHONG SHADER


out vec4 FragColor;

struct Material {
    bool hasTexture;

    vec3 albedo;
    sampler2D albedoTex;

    float roughness;
    sampler2D roughnessTex;

    bool hasMetallicTex;
    float metallic;
    sampler2D metallicTex;

    bool hasNormalMap;
    sampler2D normalMap;

    bool hasAOMap;
    sampler2D AOmap;
};

struct Light {  
    bool enabled;
    bool distant;
    int shadowMapId;
    vec3 position; 

    vec3 color;
};

in vec3 FragPos_in;
in vec2 TexCoords_in;
in vec3 Normal_in;
in vec4 FragPos_lightSpace_in[5];
  
uniform vec3 viewPos;
uniform vec2 screenSize;
uniform float exposure;
uniform vec2 texScaling;

// SSAO
uniform bool SSAOenabled;
uniform sampler2D SSAOTexture;

uniform sampler2D shadowMap[5];

uniform Light lights[NR_LIGHTS];
uniform Material material;


float ComputeShadow(vec3 lightDir, int sMapId);
vec3 CalcNormalMap(vec3 viewDir);
vec3 calcLo(Light light, vec3 viewDir, vec3 F0);
float DistributionGGX(vec3 H);
float Smith(vec3 V, vec3 L);
vec3 fresnel(float cosTheta, vec3 F0);

// pbr params
const float PI = 3.14159265359;

vec3 albedo;
float roughness;
float metallic;
vec3 normal;
float AO;



void main()
{
    vec3 viewDir = normalize(viewPos - FragPos_in);

    if(material.hasTexture){
        albedo = pow(texture(material.albedoTex,TexCoords_in*texScaling).rgb,vec3(gamma));
        roughness = pow(texture(material.roughnessTex,TexCoords_in*texScaling).r,gamma);
    } else {
        albedo = material.albedo;
        roughness = material.roughness;
    }
    roughness = max(roughness,0.02);
    albedo = vec3(1.0) - exp(-albedo * exposure);
    
    if(material.hasMetallicTex){
        metallic = texture(material.metallicTex,TexCoords_in*texScaling).r;
    } else {
        metallic = material.metallic;
    }

    if(SSAOenabled){
        if(material.hasAOMap){
            AO = texture(material.AOmap,TexCoords_in*texScaling).r;
        } else {
            vec2 ScreenCoords = vec2(gl_FragCoord.x/screenSize.x,gl_FragCoord.y/screenSize.y);
            AO = texture(SSAOTexture,ScreenCoords).r;
        }
    } else {
        AO = 1.0;
    }

    if(material.hasNormalMap){
        normal = CalcNormalMap(viewDir);
    } else{
        normal = normalize(Normal_in);
    }

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
    
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < NR_LIGHTS; i++)
        if(lights[i].enabled)
            Lo += calcLo(lights[i], viewDir, F0);    

    vec3 ambient = vec3(0.03) * albedo * AO;
    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/gamma));

    FragColor = vec4(color, 1.0);
}

vec3 calcLo(Light light, vec3 viewDir, vec3 F0){
    vec3 lightDir = normalize(light.position - FragPos_in);
    vec3 H = normalize(viewDir + lightDir);
    
    float attenuation;
    if(light.shadowMapId<0 || light.distant){
        attenuation = 1.0; // no light attenuation for sun
        
    } else {
        float dist = length(light.position - FragPos_in);
        attenuation = 1.0 / (dist*dist);
    }
    
    vec3 radiance = light.color * attenuation;

    float NDF = DistributionGGX(H);
    float G   = Smith(viewDir, lightDir);
    vec3 fresnel = fresnel(max(dot(H, viewDir), 0.0), F0);

    vec3 diffuse = vec3(1.0) - fresnel;
    diffuse *= 1.0 - metallic;

    vec3 num = (NDF * G * fresnel);
    float den = 4 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir),0.0) + 0.0001;
    vec3 specular = num / den;

    float iAngle = max(dot(normal, lightDir), 0.0);

    vec3 lo = (diffuse*albedo / PI + specular) * radiance * iAngle;

    float shadow = 0;
    if(light.shadowMapId>=0){
        shadow = ComputeShadow(lightDir,light.shadowMapId);
    }

    return lo*(1.0-shadow);
}   


float Rand(vec2 co){
  return 1/exp(fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453));
}

float ComputeShadow(vec3 lightDir, int sMapId){
    vec3 projCoords = FragPos_lightSpace_in[sMapId].xyz / FragPos_lightSpace_in[sMapId].w;
    projCoords = projCoords * 0.5 + 0.5;

    //depth bias against acne
    float bias = max(0.006 * (1.0 - dot(Normal_in, lightDir)), 0.006);


    //get half of tex res for sampling
    float offset = textureSize(shadowMap[sMapId],0).x*0.25;
    
    float lightDepth;
    float shadow = 0;
    // test for each sample point
    int samples = 16;
    for(int i = 0; i<samples; i++){

        vec2 rnd = vec2(Rand(projCoords.xy*float(i)), Rand(projCoords.yx*float(i+1)));

        lightDepth = texture(shadowMap[sMapId], projCoords.xy+(rnd/offset) ).x;
        if(projCoords.z > lightDepth  + bias){
            shadow += 1/float(samples);
        }
    }

    return shadow;  
}

vec3 CalcNormalMap(vec3 viewDir){
    viewDir = normalize(viewDir);

    vec3 texNormal = texture(material.normalMap,TexCoords_in*texScaling).xyz;
    texNormal = (texNormal * 2) - 1;

    vec3 camDir = -viewDir;
    vec3 dp1 = dFdx(camDir);
    vec3 dp2 = dFdy(camDir);
    vec2 duv1 = dFdx(TexCoords_in);
    vec2 duv2 = dFdy(TexCoords_in);

    vec3 dp2perp = cross( dp2, Normal_in );
    vec3 dp1perp = cross( Normal_in, dp1 );
    vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;

    float invmax = inversesqrt( max( dot(T,T), dot(B,B) ) );
    mat3 TBN =  mat3( T * invmax, B * invmax, Normal_in );

    return normalize(TBN*texNormal);
}

float DistributionGGX(vec3 H){
    float a = roughness*roughness;

    float NdotH2 = pow(max(dot(normal, H), 0.0),2.0);

    float b = (NdotH2 * (a*a - 1.0) + 1.0);
    b = PI * b * b;

    return (a*a) / b;
}

float SchlickGGX(float NdV)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    return NdV / (NdV * (1.0 - k) + k);
}

float Smith(vec3 V, vec3 L)
{
    float NdV = max(dot(normal, V), 0.0);
    float NdL = max(dot(normal, L), 0.0);
    float ggx2 = SchlickGGX(NdV);
    float ggx1 = SchlickGGX(NdL);

    return ggx1 * ggx2;
}

vec3 fresnel(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}