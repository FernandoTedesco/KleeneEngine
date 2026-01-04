#include "ParticleManager.h"
#include <glad/glad.h>
#include <vector>
ParticleManager::ParticleManager(Shader* shader, unsigned int textureID, unsigned int amount)
    : shader(shader), textureID(textureID), amount(amount)
{
    this->init();
    this->particles.resize(amount);
}
unsigned int ParticleManager::firstUnusedParticle()
{
    for (unsigned int i = lastUsedParticle; i < amount; ++i)
    {
	if (particles[i].life <= 0.0f)
	{
	    lastUsedParticle = i;
	    return i;
	}
    }
    for (unsigned int i = 0; i < lastUsedParticle; ++i)
    {
	if (particles[i].life <= 0.0f)
	{
	    lastUsedParticle = i;
	    return i;
	}
    }
    lastUsedParticle = 0;
    return 0;
}
void ParticleManager::respawnParticle(Particle& particle, glm::vec3 offset)
{
    float randomX = ((rand() % 100) - 50) / 10.0f;
    float randomZ = ((rand() % 100) - 50) / 10.0f;
    particle.Position = offset + glm::vec3(randomX, 0.0f, randomZ);
    particle.Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    particle.life = 1.0f;
    particle.Velocity = glm::vec3(0.0f, 1.0f, 0.0f) * 0.5f;
}

void ParticleManager::init()
{
    float quadVertices[] = {0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,

			    0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f};
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);

    glBufferData(GL_ARRAY_BUFFER, amount * (3 + 4) * sizeof(float), NULL, GL_STREAM_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glVertexAttribDivisor(1, 1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribDivisor(2, 1);
    glBindVertexArray(0);
}
void ParticleManager::Update(float dt, unsigned int newParticles, glm::vec3 offset)
{
    for (unsigned int i = 0; i < newParticles; ++i)
    {
	int unusedParticle = firstUnusedParticle();
	respawnParticle(particles[unusedParticle], offset);
    }
    for (unsigned int i = 0; i < amount; ++i)
    {
	Particle& p = particles[i];
	p.life -= dt;
	if (p.life > 0.0f)
	{
	    p.Position -= p.Velocity * dt;
	    p.Color.a -= dt * 2.5f;
	}
    }
}

void ParticleManager::Draw()
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    shader->Use();
    std::vector<float> gpuData;
    gpuData.reserve(amount * 7);
    unsigned int activeCount = 0;
    for (unsigned int i = 0; i < amount; ++i)
    {
	Particle& p = particles[i];
	if (p.life > 0.0f)
	{
	    gpuData.push_back(p.Position.x);
	    gpuData.push_back(p.Position.y);
	    gpuData.push_back(p.Position.z);

	    gpuData.push_back(p.Color.r);
	    gpuData.push_back(p.Color.g);
	    gpuData.push_back(p.Color.b);
	    gpuData.push_back(p.Color.a);

	    activeCount++;
	}
    }
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, activeCount * 7 * sizeof(float), gpuData.data());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    shader->SetInt("sprite", 0);

    glBindVertexArray(VAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, activeCount);
    glBindVertexArray(0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
ParticleManager::~ParticleManager()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteBuffers(1, &instanceVBO);
}