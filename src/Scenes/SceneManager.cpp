#include "SceneManager.h"
#include <filesystem>
#include <ios>
#include <fstream>
#include "glm/glm.hpp"
#include <iostream>
#include "Resources/ResourceManager.h"
#include "Scenes/GameObject.h"
#include "Graphics/Material.h"
bool SceneManager::LoadScene(std::filesystem::path fileName, Scene& targetScene, ResourceManager* resourceManager)
{
    std::filesystem::path currentPath = ResourceManager::FolderFinder("assets");
    std::filesystem::path fullPath = currentPath/"assets/scenes"/fileName;
    std::ifstream sceneStream(fullPath, std::ios::binary);


    if(!sceneStream)
    {
        std::cout<<"[ERROR] Failed to open file for loading:"<<fileName<<std::endl;
        return false;

    }
    uint16_t signature, version;
    uint32_t count;

    sceneStream.read(reinterpret_cast<char*>(&signature),sizeof(uint16_t));
    if(signature != 0x4B4C) return false;
    sceneStream.read(reinterpret_cast<char*>(&version),sizeof(uint16_t));
    if(version!=0x0003)
    {
        std::cout<<"[ERROR] Version mismatch during file loading";
        return false;
    }
    sceneStream.read(reinterpret_cast<char*>(&count),sizeof(uint32_t));
    targetScene.gameObjects.clear();
    targetScene.gameObjects.reserve(count);
    std::filesystem::path modelsPath = currentPath / "assets/models";
    std::filesystem::path texturesPath = currentPath / "assets/textures";
    for(uint32_t i =0;i<count;i++)
    {
        GameObject newObject;
        sceneStream.read(reinterpret_cast<char*>(&newObject.position),sizeof(glm::vec3));
        sceneStream.read(reinterpret_cast<char*>(&newObject.scale),sizeof(glm::vec3));
        sceneStream.read(reinterpret_cast<char*>(&newObject.rotation),sizeof(glm::vec4));

        uint32_t stringLen;
        std::string meshName;
        sceneStream.read(reinterpret_cast<char*>(&stringLen),sizeof(uint32_t));
        meshName.resize(stringLen);
        sceneStream.read(&meshName[0],stringLen);

        std::string textureName;
        sceneStream.read(reinterpret_cast<char*>(&stringLen),sizeof(uint32_t));
        textureName.resize(stringLen);
        sceneStream.read(&textureName[0],stringLen);

        newObject.meshID = resourceManager->CreateMesh(meshName,modelsPath/meshName);
        uint32_t textureID = resourceManager->CreateTexture(textureName,texturesPath/textureName);
        std::string materialName = "Mat_"+ textureName;
        newObject.materialID = resourceManager->CreateMaterial(materialName, textureID);
        newObject.name = meshName;
        newObject.isActive = true;
        targetScene.gameObjects.push_back(newObject);

    }
    std::cout<<"[SUCESS] Scene loaded:"<< fileName<<std::endl;
    return true;

}


bool SceneManager::SaveScene(std::filesystem::path fileName, Scene& targetScene, ResourceManager* resourceManager){

   
    std::filesystem::path currentPath = ResourceManager::FolderFinder("assets");
    std::filesystem::path fullPath = currentPath/"assets/scenes"/fileName;

    std::ofstream sceneStream(fullPath, std::ios::binary | std::ios::trunc);
    if(!sceneStream)
    {
        std::cout<<"[ERROR] Failed save file"<<fileName<<std::endl;
        return false;

    }
    uint16_t signature = 0x4B4C;
    uint16_t version = 0x0003;
    uint32_t objectCount = (uint32_t)targetScene.gameObjects.size();

    sceneStream.write(reinterpret_cast<const char*>(&signature),sizeof(uint16_t));
    sceneStream.write(reinterpret_cast<const char*>(&version),sizeof(uint16_t));
    sceneStream.write(reinterpret_cast<const char*>(&objectCount),sizeof(uint32_t));
    for(size_t i = 0; i<targetScene.gameObjects.size();i++)
    {
        const GameObject& currentObject = targetScene.gameObjects[i];

        sceneStream.write(reinterpret_cast<const char*>(&currentObject.position),sizeof(glm::vec3));
        sceneStream.write(reinterpret_cast<const char*>(&currentObject.scale),sizeof(glm::vec3));
        sceneStream.write(reinterpret_cast<const char*>(&currentObject.rotation),sizeof(glm::vec4));

        std::string meshName = resourceManager->meshNames[currentObject.meshID];
        uint32_t meshNameSize = (uint32_t)meshName.size();
        sceneStream.write(reinterpret_cast<const char*>(&meshNameSize), sizeof(uint32_t));
        sceneStream.write(meshName.c_str(),meshNameSize);

        std::string textureName = "Default.png";
        Material* objectMaterial = resourceManager->GetMaterial(currentObject.materialID);
        if(objectMaterial != nullptr && objectMaterial->diffuseMap != nullptr)
        {
            for(size_t t = 0; t <resourceManager->textureNames.size(); t++)
            {
                if(resourceManager->GetTexture((uint32_t)t)==objectMaterial->diffuseMap)
                {
                    textureName = resourceManager->textureNames[t];
                    break;
                }
            }
            
        }
        uint32_t textureNameSize = (uint32_t)textureName.size();
        sceneStream.write(reinterpret_cast<const char*>(&textureNameSize),sizeof(uint32_t));
        sceneStream.write(textureName.c_str(), textureNameSize);

    }
    sceneStream.close();
    std::cout<<"[SUCCESS] Scene saved sucessfully: "<<fileName<<std::endl;
    return true; 
    
}   


void SceneManager::AddObject(Scene& targetScene, glm::vec3 position, uint32_t meshID, uint32_t materialID)
{
    GameObject newObject;
    newObject.position = position;
    newObject.meshID = meshID;
    newObject.materialID = materialID;
    newObject.scale = glm::vec3(1.0f,1.0f,1.0f);
    newObject.rotation = glm::vec4(0.0f,0.0f,0.0f, 1.0f);
    newObject.isActive = true;
    newObject.name = "Object_" + std::to_string(targetScene.gameObjects.size());
    targetScene.gameObjects.push_back(newObject);

}