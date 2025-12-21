#pragma once
#include <filesystem>
#include "Scene.h"
class SceneManager 
{
    public:
    bool LoadScene(std::filesystem::path fileName, Scene& targetScene);
    bool SaveScene(std::filesystem::path fileName, Scene& targetScene);
    private:
    
};