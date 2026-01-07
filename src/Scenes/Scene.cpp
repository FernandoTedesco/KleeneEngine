#include "Scene.h"

Scene::Scene()
{
    skybox = nullptr;
    particleManager = nullptr;
}

Scene::~Scene()
{
    Clear();
}

void Scene::Update(float dt)
{
    for (size_t i = 0; i < gameObjects.size(); i++)
    {
	GameObject* gameObject = gameObjects[i];
	if (gameObject != nullptr && gameObject->isActive)
	{
	    gameObject->Update(dt);
	}
    }
    if (!pendingDestruction.empty())
    {
	for (size_t i = 0; i < pendingDestruction.size(); i++)
	{
	    GameObject* object = pendingDestruction[i];
	    std::vector<GameObject*>::iterator it =
		std::find(gameObjects.begin(), gameObjects.end(), object);
	    if (it != gameObjects.end())
	    {
		gameObjects.erase(it);
	    }
	    delete object;
	}
	pendingDestruction.clear();
    }
}

GameObject* Scene::CreateGameObject(const std::string& name = "New GameObject")
{
    GameObject* gameObject = new GameObject();
    gameObject->name = name;
    gameObjects.push_back(gameObject);
    return gameObject;
}
void Scene::RemoveGameObject(GameObject* gameObject)
{
    std::vector<GameObject*>::iterator it =
	std::find(pendingDestruction.begin(), pendingDestruction.end(), gameObject);

    if (it == pendingDestruction.end())
    {
	pendingDestruction.push_back(gameObject);
    }
}
void Scene::Clear()
{
    if (skybox != nullptr)
    {
	delete skybox;
	skybox = nullptr;
    }
    if (particleManager != nullptr)
    {
	delete particleManager;
	particleManager = nullptr;
    }
    for (size_t i = 0; i < gameObjects.size(); i++)
    {
	if (gameObjects[i] != nullptr)
	{
	    delete gameObjects[i];
	}
    }
    gameObjects.clear();
    skyboxPaths.clear();
    pendingDestruction.clear();
}