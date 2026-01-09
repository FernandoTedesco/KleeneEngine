#pragma once
#include <glm/glm.hpp>
#include "Scenes/Scene.h"
#include "Core/Camera.h"
#include "Core/Input.h"
class GameObject;

class PlayerController : public Component
{
public:
    PlayerController(GameObject* characterObject);
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