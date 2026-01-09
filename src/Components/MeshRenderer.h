#pragma once
#include "Component.h"
#include <glm/glm.hpp>
class MeshRenderer : public Component
{
public:
    glm::vec2 textureTiling = glm::vec2(1.0f, 1.0f);
    glm::vec2 textureOffset = glm::vec2(0.0f, 0.0f);

    uint32_t meshID;
    uint32_t materialID;
    glm::vec3 colorTint;
    bool castShadows;
    bool receiveShadows;
    bool isVisible;
    MeshRenderer()
    {
	meshID = -1;
	materialID = -1;
	colorTint = glm::vec3(1.0f);
	castShadows = true;
	receiveShadows = true;
	isVisible = true;
    }
    void SetMesh(uint32_t id)
    {
	this->meshID = id;
    }
    void SetMaterial(uint32_t id)
    {
	this->materialID = id;
    }
    void Render() override
    {
    }
};