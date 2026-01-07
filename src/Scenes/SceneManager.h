#pragma once
#include <string>
#include <filesystem>
#include "glm/glm.hpp"

class Scene;
class ResourceManager;
class GameObject;
class SceneManager
{
public:
    static bool SaveScene(std::filesystem::path fileName, Scene& targetScene,
			  ResourceManager* resourceManager);
    static bool LoadScene(std::filesystem::path fileName, Scene& targetScene,
			  ResourceManager* resourceManager);
    static GameObject* AddObject(Scene& targetScene, std::string name, uint32_t meshID,
				 uint32_t materialID, glm::vec3 position);
};