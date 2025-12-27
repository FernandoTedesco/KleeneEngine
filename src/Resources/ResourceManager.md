# Resource Manager

Version (0.5.X)

Files: ResourceManager.h / ResourceManager.cpp

## OverView

**Brief:** Runtime fast acess to mesh, texture and material O(1) using vector id allocation. Safeguards against possible resource duplicates.

---

This class is basically a sort of cache for different types of meshes, textures, and materials. It uses the following sequence:

Name->ID  and then ID -> Pointer

The idea is:

You create a resource (mesh, texture or material) by name, using CreateMesh() / CreateTexture() / CreateMaterial() if it already exists it simply returns the same id that is already in the map. The acess during runtime is by using the ID in the respective mesh/texture/material Vector.

It also manages the life-cycle of these resources, properly deleting them inside the destructor when they are not needed anymore. 

And finally, it contains a Folderfinder necessary for system paths in many different modules across the project.

## Full Documentation

Files: ResourceManager.h / ResourceManager.cpp

Role: Life-Cycle management of meshes and materials id's.



#### ResourceManager.h

1.1 Includes

Required for proper file manipulation, and for using vector and map structures

```cpp
#pragma once
#include <string>
#include <filesystem>
#include <vector>
#include <map>
```

1.2 Forwarding

Declaring classes through forwarding for them to be used in the ID-based allocation system.

```cpp
class Mesh;

class Texture;

class Material;
```



1.3 Components

regarding the class ResourceManager

#### Public

Standard construction method

```cpp
ResourceManager();
```

Destruction method, including a vector and map cleanup to manage resource life-cycle

```cpp
~ResourceManager();
```

Create new pointers for each major resource type, requires a path to the file and a name to properly fill in the maps/vectors

```cpp
uint32_t CreateMesh(const std::string& name ,std::filesystem::path filePath);
uint32_t CreateTexture(const std::string& name ,std::filesystem::path filePath);
uint32_t CreateMaterial(const std::string& name, uint32_t textureID);
```

Asks for an id and gives it's resource pointer equivalent

```cpp
Mesh* GetMesh(uint32_t id);
Texture* GetTexture(uint32_t id);
Material* GetMaterial(uint32_t id);
```



A parallel vector that saved the names of every resource created, position of name = it's ID)

```cpp
std::vector<std::string>meshNames;
std::vector<std::string>textureNames;
std::vector<std::string> materialNames;
```

#### Private

Maps any resource name to an id. ideally its all sequential so the first mesh "cube" for instance would have an id of 0 and then the next 1,2,3...

```cpp
  std::map<std::string, uint32_t> meshMap;
  std::map<std::string, uint32_t> textureMap;
  std::map<std::string, uint32_t> materialMap;
```

   

These are the vectors where all the actual resources are saved, only a pointer to their memory location is stored.

```cpp
    std::vector<Mesh*> meshVector;
    std::vector<Texture*> textureVector;
    std::vector<Material*> materialVector;
```

#### ResourceManager.cpp

Includes here are straightforward, all the resource headers are necessary so its possible to possibly use their functions to actually set up the resources before placing them into storage.

```cpp
#include <filesystem>
#include "iostream"
#include "ResourceManager.h"
#include "Graphics/Mesh.h"
#include "Graphics/Texture.h"
#include "Graphics/Material.h"
```

Currently only for looks really, but it might be needed sometime soon.

```cpp
ResourceManager::ResourceManager(){
 }
```

Allocates a new mesh into  the vectors/map

```cpp
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
```

Allocates a new texture into the vectors/map

```cpp
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
```

Allocates a new maerial into the vectors/map

```cpp
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
```



Given an ID returns the mesh in that index with O(1) speed

```cpp
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
```

Given an ID returns the texture in that index with O(1) speed

```cpp
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
```

Given an ID returns the material in that index with O(1) speed

```cpp
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
```

This is lazy and im aware it will have to be changed soon, its a really a cycles waste everytime its called but it is so insignificant in this stage that im willing to overlook it for now, but the idea in the future is to only search this once and save it for everyone to use, due to coupling issues though this is postponed.

```cpp
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
```

Destructor, important mention that the vectors indexes are cleared to ensure no trash in memory stays in the indexes, good practice.

```cpp
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
```
