#include "SpriteRenderer.h"
#include "Core/Camera.h"
#include "Scenes/GameObject.h"
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

SpriteRenderer::SpriteRenderer()
{
}

SpriteRenderer::~SpriteRenderer()
{
}

void SpriteRenderer::HandleBillboarding()
{
    glm::vec3 camPosition = targetCamera->GetCameraPos();
    glm::vec3 myPosition = owner->position;

    float deltaX = camPosition.x - myPosition.x;
    float deltaZ = camPosition.z - myPosition.z;

    float angleRadians = std::atan2(deltaX, deltaZ);
    float angleDegrees = glm::degrees(angleRadians);

    owner->rotation.x = 0.0f;
    owner->rotation.y = angleDegrees;
    owner->rotation.z = 0.0f;
}

void SpriteRenderer::Update(float dt)
{
    if (isBillboard && targetCamera && owner)
    {
	HandleBillboarding();
    }
}
