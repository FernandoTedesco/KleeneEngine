#pragma once
#include <filesystem>
#include "Scene.h"

class ResourceManager;
class SceneManager
{
public:
    static bool SaveScene(std::filesystem::path fileName, Scene& targetScene,
			  ResourceManager* resourceManager);
    static bool LoadScene(std::filesystem::path fileName, Scene& targetScene,
			  ResourceManager* resourceManager);
};