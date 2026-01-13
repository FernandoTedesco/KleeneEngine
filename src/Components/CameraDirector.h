#pragma once
#include "Component.h"
#include "Core/Camera.h"
#include "Scenes/GameObject.h"

class CameraDirector: public Component
{
    public:
    Camera* globalCameraRef = nullptr;
    glm::vec3 offset = glm::vec3(0.0f,10.0f,8.0f);
    float smoothSpeed = 5.0f;
    void Start() override
    {

    }
    void Update(float dt) override
    {
        if(!globalCameraRef || !owner) return;

        glm::vec3 desiredPos = owner->position + offset;
        glm::vec3 currentPos = globalCameraRef->GetCameraPos();
        glm::vec3 smoothedPos = glm::mix(currentPos, desiredPos, smoothSpeed *dt);

        globalCameraRef->SetCameraPosition(smoothedPos);

    }

};