#include "GameObject.h"

uint32_t GameObject::globalIDCounter = 1;

GameObject::GameObject()
{

    this->id = globalIDCounter++;
    this->name = "GameObject " + std::to_string(id);

    this->isActive = true;
    this->isStatic = false;
    this->isDirty = true;

    this->position = glm::vec3(0.0f);
    this->rotation = glm::vec3(0.0f);
    this->scale = glm::vec3(1.0f);

    this->cachedModelMatrix = glm::mat4(1.0f);
}

GameObject::~GameObject()
{
    for (Component* component : components)
    {
	delete component;
    }
    components.clear();
}
void GameObject::Update(float dt)
{
    if (!isActive)
	return;
    for (Component* component : components)
    {
	if (component->enabled)
	{
	    component->Update(dt);
	}
    }
}

glm::mat4 GameObject::GetModelMatrix()
{
    if (!isDirty)
    {
	return cachedModelMatrix;
    }
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    glm::quat orientation = glm::quat(glm::radians(rotation));
    glm::mat4 rotationMatrix = glm::mat4_cast(orientation);

    model *= rotationMatrix;
    model = glm::scale(model, scale);
    cachedModelMatrix = model;
    isDirty = false;
    return cachedModelMatrix;
}

void GameObject::SetPosition(const glm::vec3& position)
{
    if (this->position != position)
    {
	this->position = position;
	this->isDirty = true;
    }
}
void GameObject::SetRotation(const glm::vec3& rotation)
{
    if (this->rotation != rotation)
    {
	this->rotation = rotation;
	this->isDirty = true;
    }
}
void GameObject::SetScale(const glm::vec3& scale)
{
    if (this->scale != scale)
    {
	this->scale = scale;
	this->isDirty = true;
    }
}