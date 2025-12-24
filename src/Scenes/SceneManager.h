#pragma once
#include <filesystem>
#include "Scene.h"
class SceneManager 
{
    public:
    bool LoadScene(std::filesystem::path fileName, Scene& targetScene);
    bool SaveScene(std::filesystem::path fileName, Scene& targetScene);
    void AddObject(Scene& targetScene, glm::vec3 position, uint32_t meshIndex, uint32_t textureIndex);
    private:
    
};