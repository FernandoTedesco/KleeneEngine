#pragma once
#include <filesystem>
#include "Scene.h"
class ResourceManager;
class SceneManager
{
public:
    bool LoadScene(std::filesystem::path fileName, Scene& targetScene,
		   ResourceManager* resourceManager);
    bool SaveScene(std::filesystem::path fileName, Scene& targetScene,
		   ResourceManager* resourceManager);
    void AddObject(Scene& targetScene, glm::vec3 position, uint32_t meshID, uint32_t materialID);
    void DeleteObject(Scene& targetScene, int objectIndex);

private:
};