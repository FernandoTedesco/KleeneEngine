#include "SceneManager.h"
#include <filesystem>
#include <ios>
#include <fstream>
#include "glm/glm.hpp"
#include <iostream>
#include "Resources/ResourceManager.h"

bool SceneManager::LoadScene(std::filesystem::path fileName, Scene& targetScene)
{
    std::filesystem::path currentPath = ResourceManager::FolderFinder("assets");
    std::ifstream sceneStream(currentPath/"assets/scenes"/fileName, std::ios::binary);



    if(!sceneStream){

        std::cout<<"[ERROR] Could not create sceneStream"<<std::endl;
        return false;
    }
    else{
        uint16_t signature;
        uint16_t version;
        uint32_t count;
        sceneStream.read(reinterpret_cast<char*>(&signature), 2);
        
        if(signature == 0x4B4C) 
        {
            
            sceneStream.read(reinterpret_cast<char*>(&version), 2);
            if(version == 0x0001)
            {
                
                sceneStream.read(reinterpret_cast<char*>(&count),4);
                targetScene.scenePositions.resize(count);
                sceneStream.read(reinterpret_cast<char*>(targetScene.scenePositions.data()),count*sizeof(glm::vec3));
                targetScene.sceneScales.resize(count);
                sceneStream.read(reinterpret_cast<char*>(targetScene.sceneScales.data()),count*sizeof(glm::vec3));
                targetScene.sceneRotations.resize(count);
                sceneStream.read(reinterpret_cast<char*>(targetScene.sceneRotations.data()),count*sizeof(glm::vec4));
                targetScene.sceneMeshes.resize(count);
                sceneStream.read(reinterpret_cast<char*>(targetScene.sceneMeshes.data()),count*sizeof(uint32_t));
                targetScene.sceneTextures.resize(count);
                sceneStream.read(reinterpret_cast<char*>(targetScene.sceneTextures.data()),count*sizeof(uint32_t));
                std::cout<<"[SUCCESS] Scene "<< fileName << "loaded successfully!"<<std::endl;
                return true;
            }   
            else
            {
                std::cout<<"[ERROR] Invalid file version"<<std::endl;

                return false;
            }
        }
        else
        {
            std::cout<<"[ERROR] Invalid file signature"<<std::endl;
            return false;
        }
    }
}

bool SceneManager::SaveScene(std::filesystem::path fileName, Scene& targetScene){

    std::filesystem::path currentPath = ResourceManager::FolderFinder("assets");
    std::ofstream sceneStream(currentPath/"assets/scenes"/fileName, std::ios::binary| std::ios::trunc);
    if(!sceneStream)
    {
        std::cout<<"[ERROR] sceneStream failed to initialize"<<std::endl;
        return false;
    }
    else
    {
        uint16_t signature = 0x4B4C;
        sceneStream.write(reinterpret_cast<const char*>(&signature), sizeof(signature));

        uint16_t version = 0x0001;
        sceneStream.write(reinterpret_cast<const char*>(&version), sizeof(version));

        uint32_t count = targetScene.scenePositions.size();
        sceneStream.write(reinterpret_cast<const char*>(&count), sizeof(count));

        sceneStream.write(reinterpret_cast<const char*>(targetScene.scenePositions.data()), count * sizeof(glm::vec3));
        sceneStream.write(reinterpret_cast<const char*>(targetScene.sceneScales.data()), count * sizeof(glm::vec3));
        sceneStream.write(reinterpret_cast<const char*>(targetScene.sceneRotations.data()), count * sizeof(glm::vec4));

        sceneStream.write(reinterpret_cast<const char*>(targetScene.sceneMeshes.data()), count * sizeof(uint32_t));
        sceneStream.write(reinterpret_cast<const char*>(targetScene.sceneTextures.data()), count * sizeof(uint32_t));
        sceneStream.close();
        std::cout<<"[SUCCESS] File saved sucessfully as"<< fileName <<" !"<<std::endl;
        return true;
    }
    
    
}   


void SceneManager::AddObject(Scene& targetScene, glm::vec3 position, uint32_t meshIndex, uint32_t textureIndex)
{
    targetScene.scenePositions.push_back(position);
    targetScene.sceneScales.push_back(glm::vec3(1.0f,1.0f,1.0f));
    targetScene.sceneRotations.push_back(glm::vec4(0.0f,0.0f,0.0f,1.0f));
    targetScene.sceneMeshes.push_back(meshIndex);
    targetScene.sceneTextures.push_back(textureIndex);
}