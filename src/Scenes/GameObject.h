#pragma once
#include <glm/glm.hpp>
#include <string>

struct GameObject {
    std::string name;
    bool isActive;
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec4 rotation;

    uint32_t meshID;
    uint32_t materialID;
    GameObject();
};