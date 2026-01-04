#version 330 core
out vec4 FragColor;
in vec3 TexCoords;
uniform samplerCube skybox;
void main()
{
    vec4 texColor = texture(skybox, TexCoords);
    vec3 envColor = texColor.rgb;
    envColor = envColor* 0.5;
    FragColor = vec4(envColor, 1.0);
}