#include "PlayerController.h"
#include "Scenes/GameObject.h"
#include "Core/Input.h"
#include <cmath>
PlayerController::PlayerController()
{
    this->moveSpeed = 5.0f;
}
void PlayerController::Update(float dt)
{
    if (!owner)
	return;
    HandleMovement(dt);
}
void PlayerController::HandleMovement(float dt)
{
    glm::vec3 direction(0.0f);
    if (Input::IsKeyDown(Input::W_KEY))
	direction.z -= 1.0f;
    if (Input::IsKeyDown(Input::S_KEY))
	direction.z += 1.0f;
    if (Input::IsKeyDown(Input::A_KEY))
	direction.x -= 1.0f;
    if (Input::IsKeyDown(Input::D_KEY))
	direction.x += 1.0f;
    if (glm::length(direction) > 0.0f)
    {
	direction = glm::normalize(direction);
	glm::vec3 newPos = owner->position + (direction * moveSpeed * dt);
	owner->SetPosition(newPos);
    }
}
