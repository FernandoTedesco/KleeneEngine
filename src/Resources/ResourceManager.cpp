#include "ResourceManager.h"
#include <filesystem>
#include "Graphics/Mesh.h"
#include "Graphics/Texture.h"
#include "StbImage/stb_image.h"


ResourceManager::ResourceManager(){
  
}
uint32_t ResourceManager::CreateMesh(const std::string& name, std::filesystem::path filePath)
{
  std::map<std::string, uint32_t>::iterator iterator;
  iterator = meshMap.find(name);
  if(iterator == meshMap.end()) 
  {
    Mesh* newMesh = new Mesh();
    uint32_t id;
    
    newMesh->LoadOBJ(filePath);
    newMesh->SetupMesh();
    meshVector.push_back(newMesh);
    id = meshVector.size() - 1;
    meshMap[name]= id;
    
    return id;
  }
  else  
  {
    return iterator->second;
  }
  
}

Mesh* ResourceManager::GetMesh(uint32_t id)
{

  if(id>=meshVector.size())
  {
    return nullptr;
  }
  else
  {
    return meshVector[id];
  }
}


Texture* ResourceManager::GetTexture(uint32_t id)
{

  if(id>=textureVector.size())
  {
    return nullptr;
  }
  else
  {
    return textureVector[id];
  }
}

uint32_t ResourceManager::CreateTexture(const std::string& name, std::filesystem::path filePath)
{
  std::map<std::string,uint32_t>::iterator iterator;
  iterator = textureMap.find(name);
  if(iterator == textureMap.end())
  {
    uint32_t id;
    Texture* newTexture = new Texture();
    newTexture->LoadTexture(filePath);
    textureVector.push_back(newTexture);
    id = textureVector.size() - 1;
    textureMap[name] = id;
    return id;
  }
  else
  {
    return iterator->second;
  }
}

std::filesystem::path ResourceManager::FolderFinder(const std::string& foldername)
{
  std::filesystem::path currentPath;
  currentPath = std::filesystem::current_path();
  
  while(true)
  {
    
   if(std::filesystem::exists(currentPath/foldername))
   {

    return(currentPath);
   }
   else if (currentPath == currentPath.parent_path()){
    return std::filesystem::path();
   }
   
   else{
    currentPath = currentPath.parent_path();
   }
     
 }
}


ResourceManager::~ResourceManager(){
  for(size_t i=0;i<meshVector.size();i++)
  {
    delete meshVector[i];
  }
  for(size_t i=0;i<textureVector.size();i++)
  {
    delete textureVector[i];
  }

   meshVector.clear();
   meshMap.clear();
   textureVector.clear();
   textureMap.clear();
}