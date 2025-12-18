#version 330 core
out vec4 FragColor;
in vec2 textureCoordinate;
in vec3 normalVector;
uniform vec3 objectColor;
uniform sampler2D texture1;
void main()
{
    FragColor = texture(texture1, textureCoordinate * 10);
}