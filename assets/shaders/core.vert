#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexture;
layout (location = 2) in vec3 aNormal;
out vec2 textureCoordinate;
out vec3 normalVector;
out vec3 FragPos;
out vec4 FragPosLightSpace;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;
void main(){
    FragPos = vec3(model * vec4(aPosition, 1.0));
    normalVector = mat3(transpose(inverse(model)))* aNormal;
    textureCoordinate = aTexture;
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
    gl_Position = projection * view *  vec4(FragPos,  1.0);

}


