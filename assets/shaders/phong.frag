#version 440 core

#define NR_LIGHTS 10
#define gamma 2.2

// PHONG SHADER


#define MAX_DEPTH_BB 16

const vec3 colorsBB[MAX_DEPTH_BB] = { vec3(244.0 / 255, 67.0 / 255, 54.0 / 255), vec3(233.0 / 255, 30.0 / 255, 99.0 / 255),
                                    vec3(156.0 / 255, 39.0 / 255, 176.0 / 255), vec3(103.0 / 255, 58.0 / 255, 183.0 / 255),
                                    vec3(63.0 / 255, 81.0 / 255, 181.0 / 255), vec3(33.0 / 255, 150.0 / 255, 243.0 / 255),
                                    vec3(3.0 / 255, 169.0 / 255, 244.0 / 255), vec3(0.0 / 255, 188.0 / 255, 212.0 / 255),
                                    vec3(0.0 / 255, 150.0 / 255, 136.0 / 255), vec3(76.0 / 255, 175.0 / 255, 80.0 / 255),
                                    vec3(139.0 / 255, 195.0 / 255, 74.0 / 255), vec3(205.0 / 255, 220.0 / 255, 57.0 / 255),
                                    vec3(255.0 / 255, 235.0 / 255, 59.0 / 255), vec3(255.0 / 255, 193.0 / 255, 7.0 / 255),
                                    vec3(255.0 / 255, 152.0 / 255, 0.0 / 255), vec3(255.0 / 255, 87.0 / 255, 34.0 / 255) };

const float ALPHA_BB = 1;

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
};

struct Light {  
    bool enabled;
    vec3 position; 

    vec3 ambient;
    vec3 color;
    vec3 specular;
    
    vec3 attenuation;
}; 

in vec3 FragPos_in;
in vec2 TexCoords_in;
in vec3 Normal_in;
  
uniform vec3 viewPos;
uniform float exposure;
uniform vec2 texScaling;
uniform int depthBB;


uniform Light lights[NR_LIGHTS];
uniform Material material;


vec3 CalcLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir);
float ComputeShadow(vec3 lightDir, int sMapId);
vec3 CalcNormalMap(vec3 normal, vec3 viewDir);
mat3 CalcTBN(vec3 normal, vec3 viewDir);
    
void main()
{
    float alpha;
    vec3 result;
    if(depthBB == -1) // Not a bounding box
    {
        vec3 viewDir = normalize(viewPos - FragPos_in);
        result = vec3(0,0,0);
        
        for(int i = 0; i < NR_LIGHTS; i++)
            if(lights[i].enabled)
                result += CalcLight(lights[i], Normal_in, FragPos_in, viewDir);    
        alpha = 1;
    }
    else // This object is a bounding box
    {
        result = colorsBB[depthBB];
        alpha = ALPHA_BB;
    }

    FragColor = vec4(result, alpha);
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
    diffuse  = light.color  * diff * diffuseColor;
    specular = light.specular * spec * specularColor;


  
    float AOfactor = 1.0;
    if(material.hasAOMap){
        AOfactor = texture(material.AOmap,TexCoords_in).r;
    } 
    ambient *= vec3(AOfactor);


    float distance    = length(light.position - fragPos);
    float attenuation = light.attenuation.x + (light.attenuation.y * distance) + (light.attenuation.z * distance * distance);

    ambient  /= attenuation;
    diffuse  /= attenuation;
    specular /= attenuation;

    return (ambient + diffuse + specular);
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