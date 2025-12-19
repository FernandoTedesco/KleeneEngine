#include "ResourceManager.h"
#include <filesystem>
#include "Graphics/Mesh.h"
#include "Graphics/Texture.h"

ResourceManager::ResourceManager(){
  
}
Mesh* ResourceManager::CreateMesh(std::string& name, std::filesystem::path filePath)
{
  std::map<std::string, Mesh*>::iterator iterator;
  iterator = meshMap.find(name);
  if(iterator == meshMap.end()) 
  {
    Mesh* newMesh = new Mesh();
    newMesh->LoadOBJ(filePath);
    newMesh->SetupMesh();
    meshMap[name]=newMesh;
    return newMesh;
  }
  else  
  {
    return iterator->second;
  }
  
};

Mesh* ResourceManager::GetMesh(std::string& name)
{
  std::map<std::string, Mesh*>::iterator iterator;
  iterator = meshMap.find(name);
  if(iterator == meshMap.end())
  {
    return nullptr;
  }
  else
  {
    return iterator->second;
  }
}

Texture* ResourceManager::CreateTexture(std::string& name, std::filesystem::path filePath)
{
  std::map<std::string,Texture*>::iterator iterator;
  iterator = textureMap.find(name);
  if(iterator == textureMap.end())
  {
    Texture* newTexture = new Texture();
    newTexture->LoadTexture(filePath);
    textureMap[name] = newTexture;
    return newTexture;
  }
  else
  {
    return iterator->second;
  }
};

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
  for(std::pair<const std::string, Mesh*>& meshPair : meshMap)
  {
    delete meshPair.second;
  }
  for(std::pair<const std::string, Texture*>& texturePair : textureMap)
  {
    delete texturePair.second;
  }
  textureMap.clear();
}