#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
struct Particle {
    glm::vec3 Position;
    glm::vec3 Velocity;
    glm::vec4 Color;
    float life;
};
class ParticleManager
{
public:
    ParticleManager(Shader* shader, unsigned int textureID, unsigned int amount);
    ~ParticleManager();
    void Update(float dt, unsigned int newParticles, glm::vec3 offset);
    void Draw();

private:
    std::vector<Particle> particles;
    unsigned int amount;
    Shader* shader;
    unsigned int textureID;
    unsigned int VAO;
    unsigned int quadVBO;
    unsigned int instanceVBO;
    unsigned int lastUsedParticle = 0;
    void init();
    unsigned int firstUnusedParticle();
    void respawnParticle(Particle& particle, glm::vec3 offset);
};