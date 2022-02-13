#version 440 core


out vec4 FragColor;

in vec3 FragPos_in;
in vec2 TexCoords_in;
in vec3 Normal_in;
  rmal, vec3 viewDir);
    
void main()
{
    FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
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