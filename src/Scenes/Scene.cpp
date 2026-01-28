#include "Scene.h"
#include "Components/CameraDirector.h"
#include "Components/SpriteRenderer.h"
#include "Components/PlayerController.h"

Scene::Scene()
{
    skybox = nullptr;
    particleManager = nullptr;
    cachedPlayer = nullptr;
    isPlaying = false;
}
void Scene::OnRuntimeStart(Camera* mainCamera)
{
    isPlaying = true;
    for (auto& obj : gameObjects)
    {
	if (auto* dir = obj->GetComponent<CameraDirector>())
	{
	    dir->globalCameraRef = mainCamera;
	}
	if (auto* sprite = obj->GetComponent<SpriteRenderer>())
	{
	    sprite->targetCamera = mainCamera;
	    sprite->debugTilt = -20.0f;
	}
	if (obj->GetComponent<PlayerController>())
	{
	    cachedPlayer = obj;
	}
    }
}
void Scene::OnRuntimeStop()
{
    isPlaying = false;
    cachedPlayer = nullptr;
}
Scene::~Scene()
{
    Clear();
}

void Scene::Update(float dt)
{
    float effectiveDt = isPlaying ? dt : 0.0f;

    for (size_t i = 0; i < gameObjects.size(); i++)
    {
	GameObject* gameObject = gameObjects[i];
	if (gameObject != nullptr && gameObject->isActive)
	{
	    gameObject->Update(effectiveDt);
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