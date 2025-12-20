#pragma once
#include <filesystem>
#include "Scene.h"
class SceneLoader 
{
    public:
    bool LoadScene(std::filesystem::path filePath, Scene& targetScene);
    
    private:
    
};