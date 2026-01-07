#pragma once
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Components/Component.h"
class GameObject
{
public:
    std::string name;
    bool isActive;
    bool isStatic;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    GameObject();
    ~GameObject();
    uint32_t GetID() const
    {
	return id;
    }
    void Update(float dt);
    glm::mat4 GetModelMatrix();
    void SetPosition(const glm::vec3& position);
    void SetRotation(const glm::vec3& rotation);
    void SetScale(const glm::vec3& scale);
    template <typename Template> Template* AddComponent()
    {
	Template* newComponent = new Template();
	newComponent->owner = this;
	components.push_back(newComponent);
	newComponent->Start();
	return newComponent;
    }
    template <typename Template> Template* GetComponent() const
    {
	for (Component* component : components)
	{
	    Template* target = dynamic_cast<T*>(component);
	    if (target != nullptr)
	    {
		return target;
	    }
	}
	return nullptr;
    }

private:
    static uint32_t globalIDCounter;
    uint32_t id;
    glm::mat4 cachedModelMatrix;
    bool isDirty;
    std::vector<Component*> components;
};
