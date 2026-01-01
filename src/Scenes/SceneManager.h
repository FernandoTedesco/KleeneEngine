#pragma once
#include <filesystem>
#include "Scene.h"
#include "Graphics/Light.h"
class ResourceManager;
class SceneManager
{
public: // could be static in the future
    bool LoadScene(std::filesystem::path fileName, Scene& targetScene,
		   ResourceManager* resourceManager);
    bool SaveScene(std::filesystem::path fileName, Scene& targetScene,
		   ResourceManager* resourceManager);
    void AddObject(Scene& targetScene, glm::vec3 position, uint32_t meshID, uint32_t materialID);
    void AddLight(Scene& targetScene, glm::vec3 position, LightType type);
    void DeleteObject(Scene& targetScene, int objectIndex);

private:
};