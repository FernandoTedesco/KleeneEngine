#pragma once
#include "Component.h"
#include <glm/glm.hpp>
class MeshRenderer : public Component
{
public:
    uint32_t meshID;
    uint32_t materialID;
    glm::vec3 colorTint;
    bool castShadows;
    bool receiveShadows;
    MeshRenderer()
    {
	meshID = 0;
	materialID = 0;
	colorTint = glm::vec3(1.0f);
	castShadows = true;
	receiveShadows = true;
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