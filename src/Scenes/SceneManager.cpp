#include "SceneManager.h"
#include <filesystem>
#include <ios>
#include <fstream>
#include "glm/glm.hpp"
#include <iostream>
#include "Resources/ResourceManager.h"
#include "GameObject.h"
#include "Components/MeshRenderer.h"
#include "Components/Light.h"
#include "Graphics/Material.h"
#include "Development/Terminal.h"
#include "Scene.h"
#include "Graphics/Skybox.h"

bool SceneManager::LoadScene(std::filesystem::path fileName, Scene& targetScene,
			     ResourceManager* resourceManager)
{
    std::filesystem::path currentPath = ResourceManager::FolderFinder("assets");
    std::filesystem::path fullPath = currentPath / "assets/scenes" / fileName;
    std::ifstream sceneStream(fullPath, std::ios::binary);

    if (!sceneStream)
    {
	Terminal::Log(LOG_ERROR, "Failed to open file for loading:");
	return false;
    }
    uint16_t signature, version;
    uint32_t count;

    sceneStream.read(reinterpret_cast<char*>(&signature), sizeof(uint16_t));
    if (signature != 0x4B4C)
	return false;
    sceneStream.read(reinterpret_cast<char*>(&version), sizeof(uint16_t));
    if (version != 0x0006)
    {
	Terminal::Log(LOG_WARNING, "Version mismatch detected, attempting to load anyway");
    }
    sceneStream.read(reinterpret_cast<char*>(&count), sizeof(uint32_t));
    targetScene.gameObjects.clear();
    targetScene.gameObjects.reserve(count);
    std::filesystem::path modelsPath = currentPath / "assets/models";
    std::filesystem::path texturesPath = currentPath / "assets/textures";
    for (uint32_t i = 0; i < count; i++)
    {
	GameObject* newObject = targetScene.CreateGameObject("LoadedObject");
	glm::vec3 position, scale;
	glm::vec4 rotation;
	sceneStream.read(reinterpret_cast<char*>(&position), sizeof(glm::vec3));
	sceneStream.read(reinterpret_cast<char*>(&scale), sizeof(glm::vec3));
	sceneStream.read(reinterpret_cast<char*>(&rotation), sizeof(glm::vec4));

	newObject->SetPosition(position);
	newObject->SetScale(scale);
	newObject->SetRotation(rotation);

	bool hasMesh;
	sceneStream.read(reinterpret_cast<char*>(&hasMesh), sizeof(bool));

	if (hasMesh)
	{
	    uint32_t stringLen;
	    std::string meshName;
	    sceneStream.read(reinterpret_cast<char*>(&stringLen), sizeof(uint32_t));
	    meshName.resize(stringLen);
	    sceneStream.read(&meshName[0], stringLen);

	    std::string textureName;
	    sceneStream.read(reinterpret_cast<char*>(&stringLen), sizeof(uint32_t));
	    textureName.resize(stringLen);
	    sceneStream.read(&textureName[0], stringLen);
	    uint32_t meshID = resourceManager->CreateMesh(meshName, modelsPath / meshName);
	    uint32_t textureID =
		resourceManager->CreateTexture(textureName, texturesPath / textureName);
	    std::string materialName = "Mat_" + textureName;
	    uint32_t materialID = resourceManager->CreateMaterial(materialName, textureID);

	    MeshRenderer* meshRenderer = newObject->AddComponent<MeshRenderer>();
	    meshRenderer->SetMesh(meshID);
	    meshRenderer->SetMaterial(materialID);

	    newObject->name = meshName;
	}
	bool hasLight;
	sceneStream.read(reinterpret_cast<char*>(&hasLight), sizeof(bool));

	if (hasLight)
	{
	    Light* light = newObject->AddComponent<Light>();
	    sceneStream.read(reinterpret_cast<char*>(&light->type), sizeof(int));
	    sceneStream.read(reinterpret_cast<char*>(&light->color), sizeof(glm::vec3));
	    sceneStream.read(reinterpret_cast<char*>(&light->intensity), sizeof(float));
	    sceneStream.read(reinterpret_cast<char*>(&light->constant), sizeof(float));
	    sceneStream.read(reinterpret_cast<char*>(&light->linear), sizeof(float));
	    sceneStream.read(reinterpret_cast<char*>(&light->quadratic), sizeof(float));
	    sceneStream.read(reinterpret_cast<char*>(&light->cutOff), sizeof(float));
	    sceneStream.read(reinterpret_cast<char*>(&light->outerCutOff), sizeof(float));
	}
    }
    bool hasSky = false;
    sceneStream.read(reinterpret_cast<char*>(&hasSky), sizeof(bool));
    if (hasSky)
    {
	targetScene.skyboxPaths.resize(6);
	for (size_t i = 0; i < 6; i++)
	{
	    uint32_t len;
	    sceneStream.read(reinterpret_cast<char*>(&len), sizeof(uint32_t));
	    targetScene.skyboxPaths[i].resize(len);
	    sceneStream.read(&targetScene.skyboxPaths[i][0], len);
	}
	targetScene.skybox = new Skybox(targetScene.skyboxPaths);
    }
    Terminal::Log(LOG_SUCCESS, "Scene loaded ");
    return true;
}

bool SceneManager::SaveScene(std::filesystem::path fileName, Scene& targetScene,
			     ResourceManager* resourceManager)
{

    std::filesystem::path currentPath = ResourceManager::FolderFinder("assets");
    std::filesystem::path fullPath = currentPath / "assets/scenes" / fileName;

    std::ofstream sceneStream(fullPath, std::ios::binary | std::ios::trunc);
    if (!sceneStream)
    {
	std::cout << "[ERROR] Failed to save file" << fileName << std::endl;
	return false;
    }
    uint16_t signature = 0x4B4C;
    uint16_t version = 0x0006;
    uint32_t objectCount = (uint32_t)targetScene.gameObjects.size();

    sceneStream.write(reinterpret_cast<const char*>(&signature), sizeof(uint16_t));
    sceneStream.write(reinterpret_cast<const char*>(&version), sizeof(uint16_t));
    sceneStream.write(reinterpret_cast<const char*>(&objectCount), sizeof(uint32_t));
    for (size_t i = 0; i < targetScene.gameObjects.size(); i++)
    {
	GameObject* currentObject = targetScene.gameObjects[i];
	glm::vec3 position = currentObject->position;
	glm::vec3 scale = currentObject->scale;
	glm::vec3 rotation = currentObject->rotation;

	sceneStream.write(reinterpret_cast<const char*>(&position), sizeof(glm::vec3));
	sceneStream.write(reinterpret_cast<const char*>(&scale), sizeof(glm::vec3));
	sceneStream.write(reinterpret_cast<const char*>(&rotation), sizeof(glm::vec4));

	MeshRenderer* meshRenderer = currentObject->GetComponent<MeshRenderer>();
	bool hasMesh = (meshRenderer != nullptr);
	sceneStream.write(reinterpret_cast<const char*>(&hasMesh), sizeof(bool));
	if (hasMesh)
	{
	    std::string meshName = "Unknow";
	    if (meshRenderer->meshID < resourceManager->meshNames.size())
		meshName = resourceManager->meshNames[meshRenderer->meshID];

	    uint32_t meshNameSize = (uint32_t)meshName.size();
	    sceneStream.write(reinterpret_cast<const char*>(&meshNameSize), sizeof(uint32_t));
	    sceneStream.write(meshName.c_str(), meshNameSize);

	    std::string textureName = "Default.png";
	    Material* objectMaterial = resourceManager->GetMaterial(meshRenderer->materialID);
	    if (objectMaterial != nullptr && objectMaterial->diffuseMap != nullptr)
	    {
		for (size_t t = 0; t < resourceManager->textureNames.size(); t++)
		{
		    if (resourceManager->GetTexture((uint32_t)t) == objectMaterial->diffuseMap)
		    {
			textureName = resourceManager->textureNames[t];
			break;
		    }
		}
	    }

	    uint32_t textureNameSize = (uint32_t)textureName.size();
	    sceneStream.write(reinterpret_cast<const char*>(&textureNameSize), sizeof(uint32_t));
	    sceneStream.write(textureName.c_str(), textureNameSize);
	}
	Light* light = currentObject->GetComponent<Light>();
	bool hasLight = (light != nullptr);
	sceneStream.write(reinterpret_cast<const char*>(&hasLight), sizeof(bool));
	if (hasLight)
	{
	    sceneStream.write(reinterpret_cast<const char*>(&light->type), sizeof(int));
	    sceneStream.write(reinterpret_cast<const char*>(&light->color), sizeof(glm::vec3));
	    sceneStream.write(reinterpret_cast<const char*>(&light->intensity), sizeof(float));
	    sceneStream.write(reinterpret_cast<const char*>(&light->constant), sizeof(float));
	    sceneStream.write(reinterpret_cast<const char*>(&light->linear), sizeof(float));
	    sceneStream.write(reinterpret_cast<const char*>(&light->quadratic), sizeof(float));
	    sceneStream.write(reinterpret_cast<const char*>(&light->cutOff), sizeof(float));
	    sceneStream.write(reinterpret_cast<const char*>(&light->outerCutOff), sizeof(float));
	}
    }
    bool hasSky = (targetScene.skybox != nullptr);
    sceneStream.write((char*)&hasSky, sizeof(bool));
    if (hasSky)
    {
	for (size_t i = 0; i < targetScene.skyboxPaths.size(); i++)
	{
	    std::string& path = targetScene.skyboxPaths[i];
	    uint32_t len = (uint32_t)path.size();
	    sceneStream.write(reinterpret_cast<const char*>(&len), sizeof(uint32_t));
	    sceneStream.write(path.c_str(), len);
	}
    }
    sceneStream.close();
    Terminal::Log(LOG_SUCCESS, "Scene saved succesfully: ");
    return true;
}
GameObject* SceneManager::AddObject(Scene& targetScene, glm::vec3 position, uint32_t meshID,
				    uint32_t materialID)
{
    GameObject* newObject = new GameObject();
    newObject->position = position;
    MeshRenderer* renderer = newObject->AddComponent<MeshRenderer>();
    renderer->SetMesh(meshID);
    renderer->SetMaterial(materialID);
    newObject->scale = glm::vec3(1.0f, 1.0f, 1.0f);
    newObject->rotation = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    newObject->isActive = true;
    newObject->name = "Object_" + std::to_string(targetScene.gameObjects.size());
    targetScene.gameObjects.push_back(newObject);
    return (newObject);
}
