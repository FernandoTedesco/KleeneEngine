#version 330 core
layout (location = 0)out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 textureCoordinate;
in vec3 normalVector;
in vec3 FragPos;
in vec4 FragPosLightSpace;

uniform vec3 objectColor;
uniform sampler2D texture1;
uniform vec3 viewPos;
uniform sampler2D shadowMap;

struct Material{
    sampler2D diffuse;
    vec2 tiling;
    vec2 offset;
    vec3 color;
    float specular;
    float shininess;
};
uniform Material material;
struct Light{
    int type;
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
    float constant;
    float linear;
    float quadratic;
    float cutOff;
    float outerCutOff;
};
#define MAX_LIGHTS 16
uniform Light lights[MAX_LIGHTS];
uniform int numLights;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    if(projCoords.z >1.0)
    {
        return 0.0;
    }
    float bias = max(0.00005 * (1.0 - dot(normal, lightDir)), 0.00005);
    float shadow = 0.0;
    vec2 texelSize = 1.0/ textureSize(shadowMap, 0);
    for(int x = -1; x <= 1;++x)
    {
        for(int y = -1; y<= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x,y) * texelSize).r;
            shadow += (projCoords.z - bias)>pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0f;
    return shadow;
}

vec3 CalcLight(Light light, vec3 normal, vec3 viewDirection, vec3 fragPos, vec3 albedo, float shadow)
    {
        vec3 lightDirection;
        float attenuation = 1.0;
        if(light.type == 0)
        {
            lightDirection = normalize(-light.direction);
        }
        else
        {
            lightDirection = normalize(light.position - fragPos);
            float distance = length(light.position - fragPos);
            attenuation = 1.0/(light.constant + light.linear * distance + light.quadratic *(distance*distance));

        }
        float spotIntensity = 1.0;
        if(light.type == 2)
        {
            float theta = dot(lightDirection, normalize(-light.direction));
            float epsilon = light.cutOff - light.outerCutOff;
            spotIntensity = clamp((theta - light.outerCutOff)/epsilon, 0.0, 1.0);
        }
        vec3 ambient = 0.05 * light.color * albedo;
        float diff = max(dot(normal,lightDirection),0.0);
        vec3 diffuse = diff * light.color * albedo;
        vec3 reflectDirection= reflect(-lightDirection, normal);
        float spec = pow(max(dot(viewDirection,reflectDirection), 0.0), material.shininess);
        vec3 specular = material.specular * spec *light.color;
        ambient *= light.intensity;
        diffuse *= light.intensity;
        specular *= light.intensity;
        if(light.type != 0)
        {
            ambient *= attenuation *spotIntensity;
            diffuse *= attenuation * spotIntensity;
            specular *= attenuation * spotIntensity;
        }
        return(ambient+(1.0-shadow) * (diffuse+specular));
    }
void main()
{
    vec2 tiledCoords = (textureCoordinate * material.tiling) + material.offset;
    vec4 texColor = texture(material.diffuse,tiledCoords);
    vec3 albedo = pow(vec3(texColor),vec3(2.2))*material.color;
    vec3 norm = normalize(normalVector);
    vec3 viewDirection = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0);
    for(int i = 0; i<numLights; i++)
    {
        float shadow = 0.0;
        if(lights[i].type == 0)
        {
            vec3 lightDir = normalize (-lights[i].direction);
            shadow = ShadowCalculation(FragPosLightSpace, norm, lightDir);
        }
        result += CalcLight(lights[i], norm, viewDirection, FragPos, albedo, shadow);
    }
    
   
    FragColor = vec4(result, 1.0);
    float brightness = dot(result,vec3(0.2126,0.7152,0.0722));
    if(brightness>1.0)
        BrightColor = vec4(result, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
    
