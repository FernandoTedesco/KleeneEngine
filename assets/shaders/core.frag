#version 330 core
out vec4 FragColor;
in vec2 textureCoordinate;
in vec3 normalVector;
uniform vec3 objectColor;
void main()
{
    FragColor = vec4(normalVector, 1.0f);
}