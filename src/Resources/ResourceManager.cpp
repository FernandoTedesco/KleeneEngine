#include "ResourceManager.h"
#include <filesystem>
#include "Graphics/Mesh.h"
#include "Graphics/Texture.h"
#include "StbImage/stb_image.h"
#include "iostream"
#include "Graphics/Material.h"

ResourceManager::ResourceManager(){
  std::filesystem::path currentPath = ResourceManager::FolderFinder("assets");

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
    meshNames.push_back(name);
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

Material* ResourceManager::GetMaterial(uint32_t id)
{
  if(id>= materialVector.size())
  {
    return nullptr;
  }
  else
  {
    return materialVector[id];
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
    textureNames.push_back(name);
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
    std::cout<<"FolderFinder fail"<<std::endl;
    return std::filesystem::path();
   }
   
   else{
    currentPath = currentPath.parent_path();
   }
     
 }
}

uint32_t ResourceManager::CreateMaterial(const std::string& name, uint32_t textureID)
{
  std::map<std::string,uint32_t>::iterator iterator;
  iterator = materialMap.find(name);

  if(iterator == materialMap.end())
  {
    
    Material* newMaterial = new Material();
    uint32_t id;

    Texture* texture = this->GetTexture(textureID);
    if(texture != nullptr)
    {
      newMaterial->diffuseMap = texture;
    }
    else
    {
      std::cout<<"[ERROR] Tried to create the material "<< name << " with invalid texture ID"<<std::endl;
    }
    materialNames.push_back(name);
    materialVector.push_back(newMaterial);

    id = materialVector.size() - 1;
    materialMap[name] = id;
    return id;
  }
  else
  {
    return iterator->second;
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
  for(size_t i=0;i<materialVector.size();i++)
  {
    delete materialVector[i];
  }

   meshVector.clear();
   textureVector.clear();
   materialVector.clear();
   meshMap.clear();
   textureMap.clear();
   materialMap.clear();
}