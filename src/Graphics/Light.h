#pragma once
#include "glm/glm.hpp"

enum LightType {

    Directional = 0,
    Point = 1,
    Spot = 2
};

struct Light {
    LightType type = Directional;

    glm::vec3 position = glm::vec3(0.0f, 5.0f, 0.0f);
    glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    float intensity = 1.0f;
    // opengl 50 meters from the wiki formula
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
    float cutOff = glm::cos(glm::radians(12.5f));
    float outerCutOff = glm::cos(glm::radians(17.5));
};