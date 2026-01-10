#pragma once
#include "Component.h"
#include "Graphics/ParticleManager.h"
#include <glm/glm.hpp>

class ParticleSystem : public Component
{
public:
    float spawnRate = 10.0f;
    glm::vec3 offset = glm::vec3(0.0f);

    ParticleSystem();
    ~ParticleSystem();
    void Update(float dt);

private:
    float spawnAccumulator = 0.0f;
};