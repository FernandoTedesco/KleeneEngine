#include "SceneLoader.h"
#include <filesystem>
#include <ios>
#include <fstream>
#include "glm/glm.hpp"
#include <iostream>
bool SceneLoader::LoadScene(std::filesystem::path filePath, Scene& targetScene)
{
    std::ifstream sceneStream(filePath, std::ios::binary);

    if(!sceneStream){

        return false;
    }
    else{
        uint16_t signature;
        uint16_t version;
        uint32_t count;
        sceneStream.read(reinterpret_cast<char*>(&signature), 2);
        std::cout<<signature<<std::endl;
        if(signature == 0x4B4C) 
        {
            std::cout<<"Signature pass"<<std::endl;
            sceneStream.read(reinterpret_cast<char*>(&version), 2);
            if(version == 0x0001)
            {
                std::cout<<"Version pass"<<std::endl;
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

                return true;
            }   
            else
            {
                std::cout<<"Version fail"<<std::endl;

                return false;
            }
        }
        else
        {
            std::cout<<"Signature fail"<<std::endl;
            return false;
        }
    }
    

    
        
    


    
}