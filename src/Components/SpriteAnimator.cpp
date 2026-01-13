#include "SpriteAnimator.h"
#include "MeshRenderer.h"
#include "Scenes/GameObject.h"
#include <iostream>

SpriteAnimator::SpriteAnimator(GameObject* owner)
{
    this->owner = owner;
}

void SpriteAnimator::SetupSpriteSheet(int columns, int rows)
{
    this->cols = columns;
    this->rows = rows;
    this->tileSize = glm::vec2(1.0f / columns, 1.0f / rows);
}

void SpriteAnimator::AddAnimation(std::string name, int startFrame, int frameCount, float speed,
				  bool loop)
{
    AnimationClip clip;
    clip.name = name;
    clip.startFrame = startFrame;
    clip.frameCount = frameCount;
    clip.speed = speed;
    clip.loop = loop;
    animations[name] = clip;
}

void SpriteAnimator::Play(std::string name)
{
    if (currentAnimationName == name)
	return;
    if (animations.find(name) != animations.end())
    {
	currentAnimationName = name;
	currentClip = &animations[name];
	currentFrameIndex = 0;
	timer = 0.0f;
    }
}

void SpriteAnimator::Update(float dt)
{
    if (!meshRenderer)
    {
	meshRenderer = owner->GetComponent<MeshRenderer>();
	if (!meshRenderer)
	    return;
	meshRenderer->textureTiling = tileSize;
    }
    if (!currentClip)
	return;

    timer += dt;
    if (timer >= currentClip->speed)
    {
	timer = 0.0f;
	currentFrameIndex++;

	if (currentFrameIndex >= currentClip->frameCount)
	{
	    if (currentClip->loop)
	    {
		currentFrameIndex = 0;

	    } else
	    {
		currentFrameIndex = currentClip->frameCount - 1;
	    }
	}
    }
    int globalFrame = currentClip->startFrame + currentFrameIndex;
    int column = globalFrame % cols;
    int row = globalFrame / cols;
    glm::vec2 offset;
    offset.x = column * tileSize.x;
    offset.y = row * tileSize.y;
    meshRenderer->textureOffset = offset;
}