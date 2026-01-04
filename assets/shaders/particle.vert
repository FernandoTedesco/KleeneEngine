#version 330 core
layout (location = 0) in vec4 vertex;
layout (location = 1) in vec3 offset;
layout (location = 2) in vec4 color;

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    float scale = 0.2;
    TexCoords = vertex.zw;
    ParticleColor = color;

    vec4 viewPos = view * vec4(offset, 1.0);
    viewPos.xy += vertex.xy * scale;
    gl_Position = projection * viewPos;

}