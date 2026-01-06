#pragma once
#include <glm/glm.hpp>
#include "Scenes/Scene.h"
#include "Core/Camera.h"
#include "Core/Input.h"

class PlayerController
{
public:
    PlayerController(GameObject* characterObject, Camera* gameCamera);
    void Update(float dt);
    glm::vec3 GetPosition() const
    {
	return character->position;
    }

private:
    GameObject* character; // sprite
    Camera* camera;
    float moveSpeed = 4.0f;
    void HandleMovement(float dt);
    void HandleBillboarding();
};