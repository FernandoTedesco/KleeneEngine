#include "SceneManager.h"
#include <filesystem>
#include <ios>
#include <fstream>
#include "glm/glm.hpp"
#include <iostream>
#include "Resources/ResourceManager.h"

bool SceneManager::LoadScene(std::filesystem::path fileName, Scene& targetScene, ResourceManager* resourceManager)
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
            if(version == 0x0002)
            {
                
                sceneStream.read(reinterpret_cast<char*>(&count),4);
                targetScene.scenePositions.resize(count);
                sceneStream.read(reinterpret_cast<char*>(targetScene.scenePositions.data()),count*sizeof(glm::vec3));
                targetScene.sceneScales.resize(count);
                sceneStream.read(reinterpret_cast<char*>(targetScene.sceneScales.data()),count*sizeof(glm::vec3));
                targetScene.sceneRotations.resize(count);
                sceneStream.read(reinterpret_cast<char*>(targetScene.sceneRotations.data()),count*sizeof(glm::vec4));
                
                targetScene.sceneMeshes.clear();
                targetScene.sceneTextures.clear();
                std::filesystem::path currentPath = ResourceManager::FolderFinder("assets");

                for(uint32_t i = 0; i<count; i++)
                {
                    uint32_t nameSize;
                    std::string meshName;
                    std::string textureName;
                    sceneStream.read(reinterpret_cast<char*>(&nameSize),sizeof(uint32_t));
                    meshName.resize(nameSize);
                    sceneStream.read(&meshName[0],nameSize);

                    sceneStream.read(reinterpret_cast<char*>(&nameSize),sizeof(uint32_t));
                    textureName.resize(nameSize);
                    sceneStream.read(&textureName[0],nameSize);

                    uint32_t MeshID = resourceManager->CreateMesh(meshName, currentPath/"assets/models"/meshName);
                    uint32_t TextureID = resourceManager->CreateTexture(textureName, currentPath/"assets/textures"/textureName);

                    targetScene.sceneMeshes.push_back(MeshID);
                    targetScene.sceneTextures.push_back(TextureID);
                }

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

bool SceneManager::SaveScene(std::filesystem::path fileName, Scene& targetScene, ResourceManager* resourceManager){

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

        uint16_t version = 0x0002;
        sceneStream.write(reinterpret_cast<const char*>(&version), sizeof(version));

        uint32_t count = targetScene.scenePositions.size();
        sceneStream.write(reinterpret_cast<const char*>(&count), sizeof(count));

        sceneStream.write(reinterpret_cast<const char*>(targetScene.scenePositions.data()), count * sizeof(glm::vec3));
        sceneStream.write(reinterpret_cast<const char*>(targetScene.sceneScales.data()), count * sizeof(glm::vec3));
        sceneStream.write(reinterpret_cast<const char*>(targetScene.sceneRotations.data()), count * sizeof(glm::vec4));

        for(uint32_t i = 0; i<count; i++)
        {
            uint32_t currentMeshID = targetScene.sceneMeshes[i];
            std::string meshName = resourceManager->meshNames[targetScene.sceneMeshes[i]];
            uint32_t meshNameSize = (uint32_t)meshName.size();

            sceneStream.write(reinterpret_cast<const char*>(&meshNameSize),sizeof(uint32_t));
            sceneStream.write(meshName.c_str(),meshNameSize);

            uint32_t currentTextureID = targetScene.sceneTextures[i];
            std::string textureName = resourceManager->textureNames[targetScene.sceneTextures[i]];
            uint32_t textureNameSize = (uint32_t)textureName.size();

            sceneStream.write(reinterpret_cast<const char*>(&textureNameSize),sizeof(uint32_t));
            sceneStream.write(textureName.c_str(),textureNameSize);




        }
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