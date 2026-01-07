#include "PlayerController.h"
#include "Scenes/GameObject.h"
#include <cmath>
PlayerController::PlayerController(GameObject* characterObject, Camera* gameCamera)
    : character(characterObject), camera(gameCamera)
{
    this->moveSpeed = 5.0f;
}
void PlayerController::Update(float dt)
{
    if (!character)
	return;
    HandleMovement(dt);
    HandleBillboarding();
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
	character->position += direction * moveSpeed * dt;
    }
}

void PlayerController::HandleBillboarding()
{
    glm::vec3 camPos = camera->GetCameraPos();
    glm::vec3 playerPos = character->position;

    float deltaX = camPos.x - playerPos.x;
    float deltaZ = camPos.z - playerPos.z;
    float angleRadians = std::atan2(deltaX, deltaZ);
    float angleDegrees = glm::degrees(angleRadians);
    character->rotation.x = 0.0f;
    character->rotation.y = angleDegrees;
    character->rotation.z = 0.0f;
}