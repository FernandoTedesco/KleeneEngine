#include "ParticleSystem.h"
#include "Graphics/ParticleManager.h"
#include "Scenes/GameObject.h"

ParticleSystem::ParticleSystem()
{
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::Update(float dt)
{
    if (!ParticleManager::instance || !owner)
	return;
    spawnAccumulator += dt;
    float timerPerParticle = 1.0f / spawnRate;

    while (spawnAccumulator >= timerPerParticle)
    {
	spawnAccumulator -= timerPerParticle;
	ParticleManager::instance->SpawnParticles(1, owner->position + offset);
    }
}