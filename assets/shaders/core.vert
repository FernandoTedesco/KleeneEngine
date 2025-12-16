#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexture;
layout (location = 2) in vec3 aNormal;
out vec2 textureCoordinate;
out vec3 normalVector;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main(){
gl_Position = projection * view * model * vec4(aPosition, 1.0);
textureCoordinate = aTexture;
normalVector = aNormal;
}


