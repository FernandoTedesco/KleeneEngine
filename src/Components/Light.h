#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Component.h"
#include "Scenes/GameObject.h"
#include <cmath>

enum class LightType { Directional = 0, Point = 1, Spot = 2 };

class Light : public Component
{
public:
    LightType type;
    glm::vec3 color;
    float intensity;
    bool castsShadows;
    float constant;
    float linear;
    float quadratic;
    float cutOff;      // inside angle
    float outerCutOff; // extern angle
    Light()
    {
	type = LightType::Directional;
	color = glm::vec3(1.0f);
	intensity = 1.0f;
	castsShadows = true;
	constant = 1.0f;
	linear = 0.9f;
	quadratic = 0.032f;
	cutOff = 12.5f;
	outerCutOff = 17.5f;
    }
    glm::vec3 GetPosition() const
    {
	if (owner)
	{
	    return owner->position;
	}
	return glm::vec3(0.0f);
    }
    glm::vec3 GetDirection() const
    {
	if (owner)
	{
	    glm::vec3 rotation = owner->rotation;
	    glm::vec3 direction;
	    direction.x = cos(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));
	    direction.y = sin(glm::radians(rotation.x));
	    direction.z = sin(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));
	    return glm::normalize(direction);
	}
	return glm::vec3(0.0f, -1.0f, 0.0f);
    }
    void Update(float dt) override
    {
    }
};