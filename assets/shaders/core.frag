#version 330 core
out vec4 FragColor;
in vec2 textureCoordinate;
in vec3 normalVector;
uniform vec3 objectColor;
uniform sampler2D texture1;
struct Material{
    sampler2D diffuse;
    vec2 tiling;
    vec3 color;
};
uniform Material material;
void main()
{
    vec2 tiledCoords = textureCoordinate * material.tiling;
    vec4  texColor = texture(material.diffuse, tiledCoords);
    FragColor = texColor*vec4(material.color, 1.0);
}