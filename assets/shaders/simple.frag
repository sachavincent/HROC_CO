#version 460 core

#define NR_LIGHTS 10

const vec3 OCCLUDER_COLOR = vec3(0.88, 0.07, 0.11);
const vec3 OCCLUDEE_COLOR = vec3(0.17, 0.03, 0.72);

out vec4 FragColor;

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
flat in uint ObjectID_in;
in vec3 Diffuse_in;

layout (binding = 0) uniform sampler2DArray textureArray;

uniform bool debugVisibility;
uniform bool visible;
uniform float exposure;
uniform vec3 viewPos;
uniform Light lights[NR_LIGHTS];

vec3 CalcLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuse);
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
    vec3 result = vec3(0,0,0);
    vec3 viewDir = normalize(viewPos - FragPos_in);
    
    for(int i = 0; i < NR_LIGHTS; i++)
        if(lights[i].enabled)
            result += CalcLight(lights[i], Normal_in, FragPos_in, viewDir, color);    

    FragColor = vec4(result, 1.0);
}

// Calculate values for point lights
vec3 CalcLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuseColor)
{
    vec3 lightDir = normalize(light.position - fragPos);
    
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);
  
    
    vec3 ambient = vec3(0,0,0);
    vec3 diffuse = vec3(0,0,0);
    vec3 specular = vec3(0,0,0);

    vec3 specularColor = vec3(1.0);
    diffuseColor = vec3(1.0) - exp(-diffuseColor * exposure);
    specularColor = vec3(1.0) - exp(-specularColor * exposure);

    ambient  = light.ambient  * diffuseColor;
    diffuse  = light.color  * diff * diffuseColor;
    specular = light.specular * spec * specularColor;


    float distance    = length(light.position - fragPos);
    float attenuation = light.attenuation.x + (light.attenuation.y * distance) + (light.attenuation.z * distance * distance);

    ambient  /= attenuation;
    diffuse  /= attenuation;
    specular /= attenuation;

    return (ambient + diffuse + specular);
}