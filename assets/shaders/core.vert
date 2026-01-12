#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexture;
layout (location = 2) in vec3 aNormal;
layout (location = 3)in vec3 aTangent;
layout (location = 4)in vec3 aBitangent;

out vec2 textureCoordinate;
out vec3 normalVector;
out vec3 FragPos;
out vec4 FragPosLightSpace;
out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;
void main(){
    FragPos = vec3(model * vec4(aPosition, 1.0));
    normalVector = mat3(transpose(inverse(model)))* aNormal;
    textureCoordinate = aTexture;
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);

    vec3 T = normalize(vec3(model*vec4(aTangent, 0.0)));
    vec3 B = normalize(vec3(model*vec4(aBitangent, 0.0)));
    vec3 N = normalize(vec3(model*vec4(aNormal, 0.0)));
    TBN = mat3(T,B,N);
    gl_Position = projection * view *  vec4(FragPos,  1.0);

}


