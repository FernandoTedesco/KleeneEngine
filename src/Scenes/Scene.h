#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include "GameObject.h"
#include "Graphics/Skybox.h"
#include "Graphics/ParticleManager.h"
class Scene
{
    friend class SceneManager;

public:
    Scene();
    ~Scene();

    ParticleManager* particleManager = nullptr;

    std::vector<GameObject*> gameObjects;
    std::vector<GameObject*> pendingDestruction;

    Skybox* skybox = nullptr;
    std::vector<std::string> skyboxPaths;
    void Update(float dt);
    void Clear();
    void RemoveGameObject(GameObject* gameObject);
    GameObject* CreateGameObject(const std::string& name);
};