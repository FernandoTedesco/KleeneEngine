#pragma once
#include <glm/glm.hpp>
#include "Scenes/Scene.h"
#include "Core/Camera.h"
#include "Core/Input.h"
class GameObject;

class PlayerController : public Component
{
public:
    PlayerController();
    void Update(float dt);
    glm::vec3 GetPosition() const
    {
	return owner->position;
    }
    float moveSpeed = 4.0f;

private:
    Camera* camera;

    void HandleMovement(float dt);
    void HandleBillboarding();
};