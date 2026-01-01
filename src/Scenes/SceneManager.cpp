#include "SceneManager.h"
#include <filesystem>
#include <ios>
#include <fstream>
#include "glm/glm.hpp"
#include <iostream>
#include "Resources/ResourceManager.h"
#include "Scenes/GameObject.h"
#include "Graphics/Material.h"

bool SceneManager::LoadScene(std::filesystem::path fileName, Scene& targetScene,
			     ResourceManager* resourceManager)
{
    std::filesystem::path currentPath = ResourceManager::FolderFinder("assets");
    std::filesystem::path fullPath = currentPath / "assets/scenes" / fileName;
    std::ifstream sceneStream(fullPath, std::ios::binary);

    if (!sceneStream)
    {
	std::cout << "[ERROR] Failed to open file for loading:" << fileName << std::endl;
	return false;
    }
    uint16_t signature, version;
    uint32_t count;

    sceneStream.read(reinterpret_cast<char*>(&signature), sizeof(uint16_t));
    if (signature != 0x4B4C)
	return false;
    sceneStream.read(reinterpret_cast<char*>(&version), sizeof(uint16_t));
    if (version != 0x0004)
    {
	std::cout << "[ERROR] Version mismatch during file loading";
	return false;
    }
    sceneStream.read(reinterpret_cast<char*>(&count), sizeof(uint32_t));
    targetScene.gameObjects.clear();
    targetScene.gameObjects.reserve(count);
    std::filesystem::path modelsPath = currentPath / "assets/models";
    std::filesystem::path texturesPath = currentPath / "assets/textures";
    for (uint32_t i = 0; i < count; i++)
    {
	GameObject newObject;
	sceneStream.read(reinterpret_cast<char*>(&newObject.position), sizeof(glm::vec3));
	sceneStream.read(reinterpret_cast<char*>(&newObject.scale), sizeof(glm::vec3));
	sceneStream.read(reinterpret_cast<char*>(&newObject.rotation), sizeof(glm::vec4));

	uint32_t stringLen;
	std::string meshName;
	sceneStream.read(reinterpret_cast<char*>(&stringLen), sizeof(uint32_t));
	meshName.resize(stringLen);
	sceneStream.read(&meshName[0], stringLen);

	std::string textureName;
	sceneStream.read(reinterpret_cast<char*>(&stringLen), sizeof(uint32_t));
	textureName.resize(stringLen);
	sceneStream.read(&textureName[0], stringLen);

	newObject.meshID = resourceManager->CreateMesh(meshName, modelsPath / meshName);
	uint32_t textureID =
	    resourceManager->CreateTexture(textureName, texturesPath / textureName);
	std::string materialName = "Mat_" + textureName;
	newObject.materialID = resourceManager->CreateMaterial(materialName, textureID);
	newObject.name = meshName;
	newObject.isActive = true;
	targetScene.gameObjects.push_back(newObject);
    }
    uint32_t lightCount = 0;
    sceneStream.read(reinterpret_cast<char*>(&lightCount), sizeof(uint32_t));
    targetScene.lights.clear();
    targetScene.lights.reserve(lightCount);
    for (uint32_t i = 0; i < lightCount; i++)
    {
	Light newLight;
	sceneStream.read(reinterpret_cast<char*>(&newLight.type), sizeof(int));
	sceneStream.read(reinterpret_cast<char*>(&newLight.position), sizeof(glm::vec3));
	sceneStream.read(reinterpret_cast<char*>(&newLight.direction), sizeof(glm::vec3));
	sceneStream.read(reinterpret_cast<char*>(&newLight.color), sizeof(glm::vec3));

	sceneStream.read(reinterpret_cast<char*>(&newLight.constant), sizeof(float));
	sceneStream.read(reinterpret_cast<char*>(&newLight.linear), sizeof(float));
	sceneStream.read(reinterpret_cast<char*>(&newLight.quadratic), sizeof(float));

	sceneStream.read(reinterpret_cast<char*>(&newLight.cutOff), sizeof(float));
	sceneStream.read(reinterpret_cast<char*>(&newLight.outerCutOff), sizeof(float));
	targetScene.lights.push_back(newLight);
    }

    std::cout << "[SUCESS] Scene loaded:" << fileName << std::endl;
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
	std::cout << "[ERROR] Failed save file" << fileName << std::endl;
	return false;
    }
    uint16_t signature = 0x4B4C;
    uint16_t version = 0x0004;
    uint32_t objectCount = (uint32_t)targetScene.gameObjects.size();

    sceneStream.write(reinterpret_cast<const char*>(&signature), sizeof(uint16_t));
    sceneStream.write(reinterpret_cast<const char*>(&version), sizeof(uint16_t));
    sceneStream.write(reinterpret_cast<const char*>(&objectCount), sizeof(uint32_t));
    for (size_t i = 0; i < targetScene.gameObjects.size(); i++)
    {
	const GameObject& currentObject = targetScene.gameObjects[i];

	sceneStream.write(reinterpret_cast<const char*>(&currentObject.position),
			  sizeof(glm::vec3));
	sceneStream.write(reinterpret_cast<const char*>(&currentObject.scale), sizeof(glm::vec3));
	sceneStream.write(reinterpret_cast<const char*>(&currentObject.rotation),
			  sizeof(glm::vec4));

	std::string meshName = resourceManager->meshNames[currentObject.meshID];
	uint32_t meshNameSize = (uint32_t)meshName.size();
	sceneStream.write(reinterpret_cast<const char*>(&meshNameSize), sizeof(uint32_t));
	sceneStream.write(meshName.c_str(), meshNameSize);

	std::string textureName = "Default.png";
	Material* objectMaterial = resourceManager->GetMaterial(currentObject.materialID);
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
    uint32_t lightCount = (uint32_t)targetScene.lights.size();
    sceneStream.write(reinterpret_cast<const char*>(&lightCount), sizeof(uint32_t));
    for (const Light& light : targetScene.lights)
    {
	sceneStream.write(reinterpret_cast<const char*>(&light.type), sizeof(int));
	sceneStream.write(reinterpret_cast<const char*>(&light.position), sizeof(glm::vec3));
	sceneStream.write(reinterpret_cast<const char*>(&light.direction), sizeof(glm::vec3));
	sceneStream.write(reinterpret_cast<const char*>(&light.color), sizeof(glm::vec3));

	sceneStream.write(reinterpret_cast<const char*>(&light.constant), sizeof(float));
	sceneStream.write(reinterpret_cast<const char*>(&light.linear), sizeof(float));
	sceneStream.write(reinterpret_cast<const char*>(&light.quadratic), sizeof(float));

	sceneStream.write(reinterpret_cast<const char*>(&light.cutOff), sizeof(float));
	sceneStream.write(reinterpret_cast<const char*>(&light.outerCutOff), sizeof(float));
    }
    sceneStream.close();
    std::cout << "[SUCCESS] Scene saved sucessfully: " << fileName << std::endl;
    return true;
}
void SceneManager::AddLight(Scene& targetScene, glm::vec3 position, LightType type)
{
    Light newLight;
    newLight.type = type;
    newLight.position = position;

    newLight.color = glm::vec3(1.0f, 1.0f, 1.0f);
    newLight.intensity = 1.0f;
    newLight.direction = glm::vec3(0.0f, -1.0f, 0.0f);
    newLight.constant = 1.0f;
    newLight.linear = 0.09f;
    newLight.quadratic = 0.032f;
    newLight.cutOff = glm::cos(glm::radians(12.5));
    newLight.outerCutOff = glm::cos(glm::radians(17.5));
    targetScene.lights.push_back(newLight);
    std::cout << "[INFO]Light Created at" << position.x << ", " << position.y << std::endl;
}
void SceneManager::AddObject(Scene& targetScene, glm::vec3 position, uint32_t meshID,
			     uint32_t materialID)
{
    GameObject newObject;
    newObject.position = position;
    newObject.meshID = meshID;
    newObject.materialID = materialID;
    newObject.scale = glm::vec3(1.0f, 1.0f, 1.0f);
    newObject.rotation = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    newObject.isActive = true;
    newObject.name = "Object_" + std::to_string(targetScene.gameObjects.size());
    targetScene.gameObjects.push_back(newObject);
}

void SceneManager::DeleteObject(Scene& scene, int objectIndex)
{
    if (objectIndex >= 0 && objectIndex < scene.gameObjects.size())
    {
	scene.gameObjects.erase(scene.gameObjects.begin() + objectIndex);
	std::cout << "[INFO] Entity deleted" << std::endl;
    } else
    {
	std::cout << "[ERROR] Could not delete entity" << std::endl;
    }
}