#pragma once
#include "Components/Component.h"
#include "glm/glm.hpp"
#include <vector>
#include <string>
#include <map>
struct AnimationClip {
    std::string name;
    int startFrame;
    int frameCount;
    float speed;
    bool loop;
};
class MeshRenderer;
class SpriteAnimator : public Component
{
public:
    SpriteAnimator() = default;
    SpriteAnimator(GameObject* owner);
    void Update(float dt) override;
    void SetupSpriteSheet(int columns, int rows);
    void AddAnimation(std::string name, int startFrame, int frameCount, float speed = 0.15f,
		      bool loop = true);
    void Play(std::string name);
    MeshRenderer* meshRenderer = nullptr;
    int cols = 1;
    int rows = 1;
    glm::vec2 tileSize = glm::vec2(1.0f);

    std::string currentAnimationName = "";
    AnimationClip* currentClip = nullptr;
    float timer = 0.0f;
    int currentFrameIndex = 0;

    // Cache
    std::map<std::string, AnimationClip> animations;
};