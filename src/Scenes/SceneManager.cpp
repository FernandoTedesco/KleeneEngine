#include "SceneManager.h"
#include <iostream>
#include <fstream>
#include "Resources/ResourceManager.h"
#include "GameObject.h"

#include "Components/MeshRenderer.h"
#include "Components/Light.h"
#include "Components/Terrain.h"

#include "Development/Terminal.h"
#include "Core/Paths.h"
#include "Nlohmann/json.hpp"
#include "Graphics/Material.h"
#include "Graphics/Texture.h"
using json = nlohmann::json;

namespace {
json Vec3ToJson(const glm::vec3& v)
{
    return {v.x, v.y, v.z};
}
glm::vec3 JsonToVec3(const json& j, const glm::vec3& default = glm::vec3(0.0f))
{
    if (j.is_array() && j.size() >= 3)
    {
	return glm::vec3(j[0], j[1], j[2]);
    }
    return default;
}
json Vec2ToJson(const glm::vec2& v)
{
    return {v.x, v.y};
}
glm::vec2 JsonToVec2(const json& j)
{
    if (j.is_array() && j.size() >= 2)
    {
	return glm::vec2(j[0], j[1]);
    }
    return glm::vec2(0.0f);
}
} // namespace

bool SceneManager::SaveScene(std::filesystem::path fileName, Scene& targetScene,
			     ResourceManager* resourceManager)
{
    std::filesystem::path finalPath = Paths::Assets / "scenes" / fileName.filename();

    json root;
    root["Format"] = "KleeneEngine_JSON";
    root["Version"] = 1;
    root["ObjectCount"] = targetScene.gameObjects.size();

    json objectsArray = json::array();
    for (GameObject* object : targetScene.gameObjects)
    {
	json objectData;
	objectData["Name"] = object->name;
	objectData["ID"] = object->GetID();

	objectData["Transform"]["Position"] = Vec3ToJson(object->position);
	objectData["Transform"]["Rotation"] = Vec3ToJson(object->rotation);
	objectData["Transform"]["Scale"] = Vec3ToJson(object->scale);

	MeshRenderer* mesh = object->GetComponent<MeshRenderer>();
	if (mesh != nullptr)
	{
	    json comp;
	    std::string meshPathStr = "cube.obj";

	    if (mesh->meshID < resourceManager->meshNames.size())
	    {
		meshPathStr = resourceManager->meshPaths[mesh->meshID];
	    }

	    std::string textureFileName = "default.png";
	    Material* material = resourceManager->GetMaterial(mesh->materialID);
	    if (material && material->diffuseMap)
	    {
		for (size_t i = 0; i < resourceManager->textureVector.size(); i++)
		{
		    if (resourceManager->textureVector[i] == material->diffuseMap)
		    {
			if (i < resourceManager->texturePaths.size())
			    textureFileName =
				std::filesystem::path(resourceManager->texturePaths[i])
				    .filename()
				    .string();
			break;
		    }
		}
	    }
	    comp["MeshFile"] = meshPathStr;
	    comp["TextureFile"] = textureFileName;
	    comp["Tiling"] = Vec2ToJson(mesh->textureTiling);
	    comp["Offset"] = Vec2ToJson(mesh->textureOffset);
	    comp["Color"] = Vec3ToJson(mesh->colorTint);
	    comp["CastShadows"] = mesh->castShadows;
	    comp["ReceiveShadows"] = mesh->receiveShadows;

	    objectData["Components"]["MeshRenderer"] = comp;
	}
	Light* light = object->GetComponent<Light>();
	if (light != nullptr)
	{
	    json comp;
	    comp["Type"] = (int)light->type;
	    comp["Color"] = Vec3ToJson(light->color);
	    comp["Intensity"] = light->intensity;
	    comp["Constant"] = light->constant;
	    comp["Linear"] = light->linear;
	    comp["Quadratic"] = light->quadratic;
	    comp["CutOff"] = light->cutOff;
	    comp["OuterCutOff"] = light->outerCutOff;

	    objectData["Components"]["Light"] = comp;
	}

	Terrain* terrain = object->GetComponent<Terrain>();
	if (terrain != nullptr)
	{
	    json comp;
	    comp["Width"] = terrain->width;
	    comp["Depth"] = terrain->depth;
	    comp["TileSize"] = terrain->tileSize;

	    comp["HeightMap"] = terrain->heightMap;
	    objectData["Components"]["Terrain"] = comp;
	}
	objectsArray.push_back(objectData);
    }

    root["GameObjects"] = objectsArray;

    if (targetScene.skybox != nullptr && targetScene.skyboxPaths.size() == 6)
    {
	root["Skybox"] = targetScene.skyboxPaths;
    }
    std::ofstream file(finalPath);
    if (!file.is_open())
    {
	Terminal::Log(LOG_ERROR, "Failed to create JSON file: " + finalPath.string());
	return false;
    }
    file << root.dump(4);
    file.close();
    Terminal::Log(LOG_SUCCESS, "Scene saved successfully!");
    return true;
}

bool SceneManager::LoadScene(std::filesystem::path fileName, Scene& targetScene,
			     ResourceManager* resourceManager)
{
    std::filesystem::path finalPath = Paths::Assets / "scenes" / fileName.filename();
    std::ifstream file(finalPath);
    if (!file.is_open())
    {
	Terminal::Log(LOG_ERROR, "JSON File not found: " + finalPath.string());
	return false;
    }
    targetScene.Clear();
    try
    {
	json root;
	file >> root;
	if (root.contains("GameObjects"))
	{
	    for (const auto& objJson : root["GameObjects"])
	    {
		std::string name = objJson.value("Name", "Unnamed Object");
		GameObject* newObject = targetScene.CreateGameObject(name);
		if (objJson.contains("Transform"))
		{
		    newObject->SetPosition(JsonToVec3(objJson["Transform"]["Position"]));
		    newObject->SetRotation(JsonToVec3(objJson["Transform"]["Rotation"]));
		    json scaleJson =
			objJson["Transform"].value("Scale", json::array({1.0, 1.0, 1.0}));
		    newObject->SetScale(JsonToVec3(scaleJson, glm::vec3(1.0f)));
		}
		if (objJson.contains("Components"))
		{
		    const auto& comps = objJson["Components"];
		    if (comps.contains("MeshRenderer"))
		    {
			const auto& mData = comps["MeshRenderer"];
			std::string meshName = mData.value("MeshFile", "cube.obj");
			std::string textureName = mData.value("TextureFile", "default.png");

			uint32_t meshID =
			    resourceManager->CreateMesh(meshName, Paths::Models / meshName);
			std::filesystem::path texturePath = Paths::Textures / textureName;
			uint32_t textureID = 0;
			if (std::filesystem::exists(texturePath))
			    textureID = resourceManager->CreateTexture(textureName, texturePath);

			std::string materialName = "Mat_" + textureName;
			uint32_t materialID =
			    resourceManager->CreateMaterial(materialName, textureID);
			MeshRenderer* meshRenderer = newObject->AddComponent<MeshRenderer>();
			meshRenderer->SetMesh(meshID);
			meshRenderer->SetMaterial(materialID);

			meshRenderer->textureTiling = JsonToVec2(mData["Tiling"]);
			meshRenderer->textureOffset = JsonToVec2(mData["Offset"]);
			meshRenderer->colorTint = JsonToVec3(mData["Color"], glm::vec3(1.0f));
			meshRenderer->castShadows = mData.value("CastShadows", true);
			meshRenderer->receiveShadows = mData.value("ReceiveShadows", true);
		    }
		    if (comps.contains("Light"))
		    {
			const auto& lData = comps["Light"];
			Light* l = newObject->AddComponent<Light>();
			l->type = (decltype(l->type))lData.value("Type", 0);
			l->color = JsonToVec3(lData["Color"], glm::vec3(1.0f));
			l->intensity = lData.value("Intensity", 1.0f);
			l->constant = lData.value("Constant", 1.0f);
			l->linear = lData.value("Linear", 0.09f);
			l->quadratic = lData.value("Quadratic", 0.032f);
			l->cutOff = lData.value("CutOff", 12.0f);
			l->outerCutOff = lData.value("OuterCutOff", 15.0f);
		    }
		    if (comps.contains("Terrain"))
		    {
			const auto& tData = comps["Terrain"];
			Terrain* terrain = newObject->AddComponent<Terrain>();
			terrain->SetResourceManager(resourceManager);
			int w = tData.value("Width", 10);
			int d = tData.value("Depth", 10);
			float s = tData.value("TileSize", 1.0f);
			terrain->InitializeGrid(w, d, s);
			if (tData.contains("HeightMap"))
			{
			    std::vector<float> savedHeights =
				tData["HeightMap"].get<std::vector<float>>();
			    if (savedHeights.size() == terrain->heightMap.size())
			    {
				terrain->heightMap = savedHeights;
				terrain->isDirty = true;
			    }
			}
		    }
		}
	    }
	}
	if (root.contains("Skybox") && root["Skybox"].is_array())
	{
	    targetScene.skyboxPaths.clear();
	    for (const auto& path : root["Skybox"])
	    {
		targetScene.skyboxPaths.push_back(path.get<std::string>());
	    }
	    if (targetScene.skyboxPaths.size() == 6)
		targetScene.skybox = new Skybox(targetScene.skyboxPaths);
	}
	Terminal::Log(LOG_SUCCESS, "Scene loaded successfully");
	return true;
    } catch (json::exception& e)
    {
	std::string error = e.what();
	Terminal::Log(LOG_ERROR, "Critical error while loading file" + error);
	return false;
    }
}