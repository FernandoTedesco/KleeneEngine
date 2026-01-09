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

    void SetTargetCamera(Camera* camera);
    void Update(float dt) override;

private:
    Camera* camera = nullptr;
    void HandleBillboarding();
};