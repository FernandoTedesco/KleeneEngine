#version 330 core
out vec4 FragColor;

in vec2 textureCoordinate;
in vec3 normalVector;
in vec3 FragPos;
uniform vec3 objectColor;
uniform sampler2D texture1;

uniform vec3 viewPos;

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


vec3 CalcLight(Light light, vec3 normal, vec3 viewDirection, vec3 fragPos)
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
        vec3 ambient = 0.05 * light.color;
        float diff = max(dot(normal,lightDirection),0.0);
        vec3 diffuse = diff * light.color;
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
        return(ambient+diffuse+specular);
    }
void main()
{
    vec3 norm = normalize(normalVector);
    vec3 viewDirection = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0);
    for(int i = 0; i<numLights; i++)
    {
        result += CalcLight(lights[i], norm, viewDirection, FragPos);
    }
    vec2 tiledCoords = (textureCoordinate *material.tiling) + material.offset;
    vec4 texColor = texture(material.diffuse, tiledCoords);
    FragColor = vec4(result, 1.0)*texColor * vec4(material.color, 1.0);
    
}
    
