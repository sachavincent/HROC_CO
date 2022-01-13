#version 440 core

#define NR_LIGHTS 100
#define gamma 2.2

// PHONG SHADER


out vec4 FragColor;

struct Material {
    bool hasTexture;
    vec3 diffuse;
    vec3 specular;
    sampler2D  diffuseTex;
    sampler2D  specularTex;

    bool hasNormalMap;
    sampler2D normalMap;

    bool hasAOMap;
    sampler2D AOmap;

    float shininess;
    float specularStrength;
};

struct Light {  
    bool enabled;
    bool distant;
    int shadowMapId;   
    vec3 position; 

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float constant;
    float linear;
    float quadratic; 
}; 

in vec3 FragPos_in;
in vec2 TexCoords_in;
in vec3 Normal_in;
in vec4 FragPos_lightSpace_in[5];
  
uniform vec3 viewPos;
uniform vec2 screenSize;
uniform float exposure;
uniform vec2 texScaling;


uniform sampler2D shadowMap[5];

uniform Light lights[NR_LIGHTS];
uniform Material material;


vec3 CalcLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir);
float ComputeShadow(vec3 lightDir, int sMapId);
vec3 CalcNormalMap(vec3 normal, vec3 viewDir);
mat3 CalcTBN(vec3 normal, vec3 viewDir);
    
void main()
{
    vec3 viewDir = normalize(viewPos - FragPos_in);
    vec3 result = vec3(0,0,0);


    for(int i = 0; i < NR_LIGHTS; i++)
        if(lights[i].enabled)
            result += CalcLight(lights[i], Normal_in, FragPos_in, viewDir);    

    
    FragColor = vec4(result, 1.0);
}

// Calculate values for point lights
vec3 CalcLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    if (material.hasNormalMap){
        normal = CalcNormalMap(normal,viewDir);
    }
    
    
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  
    
    vec3 ambient = vec3(0,0,0);
    vec3 diffuse = vec3(0,0,0);
    vec3 specular = vec3(0,0,0);

    vec3 diffuseColor;
    vec3 specularColor;
    if(material.hasTexture){
        diffuseColor =texture(material.diffuseTex,TexCoords_in*texScaling).rgb;
        specularColor = texture(material.specularTex,TexCoords_in*texScaling).rgb;
    } else {
        diffuseColor = material.diffuse;
        specularColor = material.specular;
    }

    diffuseColor = vec3(1.0) - exp(-diffuseColor * exposure);
    specularColor = vec3(1.0) - exp(-specularColor * exposure);

    ambient  = light.ambient  * diffuseColor;
    diffuse  = light.diffuse  * diff * diffuseColor;
    specular = light.specular * spec * specularColor *vec3(material.specularStrength);


  
    float AOfactor = 1.0;
    if(material.hasAOMap){
        AOfactor = texture(material.AOmap,TexCoords_in).r;
    } 
    ambient *= vec3(AOfactor);


    // calculate attenuation ( constant < 0 to bypass) and distant light have no falloff
    if (light.shadowMapId<0 || !light.distant){
        float distance    = length(light.position - fragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			    light.quadratic * (distance*distance));

        ambient  *= attenuation;
        diffuse  *= attenuation;
        specular *= attenuation;
    }

    float shadow = 0;
    if(light.shadowMapId>=0){
        shadow = ComputeShadow(lightDir,light.shadowMapId);
    }

    return (ambient + (1.0-shadow)*(diffuse + specular));
}


float rand(vec2 co){
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

        vec2 rnd = vec2(rand(projCoords.xy*float(i)), rand(projCoords.yx*float(i+1)));

        lightDepth = texture(shadowMap[sMapId], projCoords.xy+(rnd/offset) ).x;
        if(projCoords.z > lightDepth  + bias){
            shadow += 1/float(samples);
        }
    }

    return shadow;  
}

vec3 CalcNormalMap(vec3 normal, vec3 viewDir){
    viewDir = normalize(viewDir);

    vec3 texNormal = texture(material.normalMap,TexCoords_in*texScaling).xyz;
    texNormal = (texNormal * 2) - 1; //from 01 to -1 1
    mat3 TBN = CalcTBN(normal, viewDir);

    return normalize(TBN*texNormal);
}

mat3 CalcTBN(vec3 normal, vec3 viewDir){
    vec3 camDir = -viewDir;
    vec3 dp1 = dFdx(camDir);
    vec3 dp2 = dFdy(camDir);
    vec2 duv1 = dFdx(TexCoords_in);
    vec2 duv2 = dFdy(TexCoords_in);

    vec3 dp2perp = cross( dp2, normal );
    vec3 dp1perp = cross( normal, dp1 );
    vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;

    float invmax = inversesqrt( max( dot(T,T), dot(B,B) ) );
    return mat3( T * invmax, B * invmax, normal );
}