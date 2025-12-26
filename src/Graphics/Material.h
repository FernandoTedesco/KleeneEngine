#pragma once
#include <glm/glm.hpp>
class Texture;
class Shader;
class Material{

    public:
    Material();
    Texture* diffuseMap = nullptr;
    glm::vec3 colorTint = glm::vec3(1.0f,1.0f,1.0f);
    glm::vec2 tiling = glm::vec2(1.0f,1.0f);
    void Use(Shader* shader);

};