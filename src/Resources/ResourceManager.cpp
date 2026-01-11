#include <filesystem>
#include "iostream"
#include "ResourceManager.h"
#include "Graphics/Mesh.h"
#include "Graphics/Texture.h"
#include "Graphics/Material.h"

// Module initializer
ResourceManager::ResourceManager()
{
}

// Creates a pointer to a mesh and stores in the class map and vectors
uint32_t ResourceManager::CreateMesh(const std::string& name, std::filesystem::path filePath)
{
    std::map<std::string, uint32_t>::iterator iterator;
    iterator = meshMap.find(name);
    if (iterator == meshMap.end())
    {
	Mesh* newMesh = new Mesh();
	uint32_t id;

	newMesh->LoadOBJ(filePath);
	newMesh->SetupMesh();
	meshNames.push_back(name);

	meshPaths.push_back(filePath.string());

	meshVector.push_back(newMesh);
	id = meshVector.size() - 1;
	meshMap[name] = id;

	return id;
    } else
    {
	return iterator->second;
    }
}

uint32_t ResourceManager::CreateMesh(const std::string& name)
{
    std::map<std::string, uint32_t>::iterator iterator;
    iterator = meshMap.find(name);
    if (iterator != meshMap.end())
    {
	return iterator->second;
    }
    Mesh* newMesh = new Mesh();
    meshNames.push_back(name);
    meshPaths.push_back(name);
    meshVector.push_back(newMesh);
    uint32_t id = (uint32_t)meshVector.size() - 1;
    meshMap[name] = id;
    return id;
}
// Creates a pointer to a texture and stores in the class map and vectors, gives the id
uint32_t ResourceManager::CreateTexture(const std::string& name, std::filesystem::path filePath)
{
    std::map<std::string, uint32_t>::iterator iterator;
    iterator = textureMap.find(name);
    if (iterator == textureMap.end())
    {
	uint32_t id;
	Texture* newTexture = new Texture();
	newTexture->LoadTexture(filePath);
	textureNames.push_back(name);
	texturePaths.push_back(filePath.string());
	textureVector.push_back(newTexture);
	id = textureVector.size() - 1;
	textureMap[name] = id;
	return id;
    } else
    {
	return iterator->second;
    }
}

// Creates a pointer to a material and stores in the class map and vectors
uint32_t ResourceManager::CreateMaterial(const std::string& name, uint32_t textureID)
{
    std::map<std::string, uint32_t>::iterator iterator;
    iterator = materialMap.find(name);

    if (iterator == materialMap.end())
    {

	Material* newMaterial = new Material();
	uint32_t id;

	Texture* texture = this->GetTexture(textureID);
	if (texture != nullptr)
	{
	    newMaterial->diffuseMap = texture;
	} else
	{
	    std::cout << "[ERROR] Tried to create the material " << name
		      << " with invalid texture ID" << std::endl;
	}
	materialNames.push_back(name);
	materialVector.push_back(newMaterial);

	id = materialVector.size() - 1;
	materialMap[name] = id;
	return id;
    } else
    {
	return iterator->second;
    }
}

// Given an id, gives the mesh stored in that specific index
Mesh* ResourceManager::GetMesh(uint32_t id)
{

    if (id >= meshVector.size())
    {
	return nullptr;
    } else
    {
	return meshVector[id];
    }
}

// Given an id, gives the texture stored in that specific index
Texture* ResourceManager::GetTexture(uint32_t id)
{

    if (id >= textureVector.size())
    {
	return nullptr;
    } else
    {
	return textureVector[id];
    }
}

// Given an id, gives the material stored in that specific index
Material* ResourceManager::GetMaterial(uint32_t id)
{
    if (id >= materialVector.size())
    {
	return nullptr;
    } else
    {
	return materialVector[id];
    }
}

// Auxiliary function, currently many modules depend on it to get the assets path
std::filesystem::path ResourceManager::FolderFinder(const std::string& foldername)
{
    std::filesystem::path currentPath;
    currentPath = std::filesystem::current_path();

    while (true)
    {

	if (std::filesystem::exists(currentPath / foldername))
	{

	    return (currentPath);
	} else if (currentPath == currentPath.parent_path())
	{
	    std::cout << "FolderFinder fail" << std::endl;
	    return std::filesystem::path();
	}

	else
	{
	    currentPath = currentPath.parent_path();
	}
    }
}

// Destroy and cleans the vectors and maps properly
ResourceManager::~ResourceManager()
{
    for (size_t i = 0; i < meshVector.size(); i++)
    {
	delete meshVector[i];
    }
    for (size_t i = 0; i < textureVector.size(); i++)
    {
	delete textureVector[i];
    }
    for (size_t i = 0; i < materialVector.size(); i++)
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