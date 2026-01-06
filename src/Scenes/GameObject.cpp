#include "GameObject.h"
#include "glm/glm.hpp"
GameObject::GameObject()
{

    name = "New Object";
    isActive = true;
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    scale = glm::vec3(1.0f, 1.0f, 1.0f);
    rotation = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    meshID = 0;
    materialID = 0;
}