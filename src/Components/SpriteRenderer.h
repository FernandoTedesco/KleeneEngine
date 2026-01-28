#pragma once
#include "Components/Component.h"
#include <glm/glm.hpp>

class Camera;

class SpriteRenderer : public Component
{
public:
    bool isBillboard = true;
    Camera* targetCamera = nullptr;

    SpriteRenderer();
    ~SpriteRenderer();
    bool isBilldBoard = true;
    bool lockY = true;
    float debugTilt = 0.0f;
    void SetTargetCamera(Camera* camera);
    void Update(float dt) override;

private:
    Camera* camera = nullptr;
    void HandleBillboarding();
};