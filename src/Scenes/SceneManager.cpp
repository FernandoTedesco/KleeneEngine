#include "SceneManager.h"
#include <iostream>
#include <fstream>
#include "Resources/ResourceManager.h"
#include "GameObject.h"

#include "Components/MeshRenderer.h"
#include "Components/Light.h"
#include "Components/Terrain.h"
#include "Components/PlayerController.h"
#include "Components/SpriteRenderer.h"
#include "Components/SpriteAnimator.h"
#include "Components/CameraDirector.h"

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
glm::vec3 JsonToVec3(const json& j, const glm::vec3& defaultValue = glm::vec3(0.0f))
{
    if (j.is_array() && j.size() >= 3)
    {
	return glm::vec3(j[0], j[1], j[2]);
    }
    return defaultValue;
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
	    std::string diffuseName = "default.png";
	    std::string normalName = "None";
	    std::string specularName = "None";

	    Material* material = resourceManager->GetMaterial(mesh->materialID);
	    if (material)
	    {
		if (material->diffuseMap)
		{
		    for (size_t i = 0; i < resourceManager->textureVector.size(); i++)
		    {
			if (resourceManager->textureVector[i] == material->diffuseMap)
			{
			    if (i < resourceManager->texturePaths.size())
				diffuseName =
				    std::filesystem::path(resourceManager->texturePaths[i])
					.filename()
					.string();
			    break;
			}
		    }
		}
		if (material->normalMap)
		{
		    for (size_t i = 0; i < resourceManager->textureVector.size(); i++)
		    {
			if (resourceManager->textureVector[i] == material->normalMap)
			{
			    if (resourceManager->textureVector[i] == material->normalMap)
			    {
				if (i < resourceManager->texturePaths.size())
				    normalName =
					std::filesystem::path(resourceManager->texturePaths[i])
					    .filename()
					    .string();
				break;
			    }
			}
		    }
		}
		if (material->specularMap)
		{
		    for (size_t i = 0; i < resourceManager->textureVector.size(); i++)
		    {
			if (resourceManager->textureVector[i] == material->specularMap)
			{
			    if (i < resourceManager->texturePaths.size())
			    {
				specularName =
				    std::filesystem::path(resourceManager->texturePaths[i])
					.filename()
					.string();
				break;
			    }
			}
		    }
		}
	    }
	    comp["MeshFile"] = std::filesystem::path(meshPathStr).filename().string();
	    comp["TextureFile"] = diffuseName;
	    comp["NormalFile"] = normalName;
	    comp["SpecularFile"] = specularName;

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
	PlayerController* playerController = object->GetComponent<PlayerController>();
	if (playerController != nullptr)
	{
	    json comp;
	    comp["MoveSpeed"] = playerController->moveSpeed;
	    objectData["Components"]["PlayerController"] = comp;
	}
	SpriteRenderer* spriteRenderer = object->GetComponent<SpriteRenderer>();
	if (spriteRenderer != nullptr)
	{
	    json comp;
	    comp["IsBillboard"] = spriteRenderer->isBillboard;
	    comp["LockY"] = spriteRenderer->lockY;
	    objectData["Components"]["SpriteRenderer"] = comp;
	}
	SpriteAnimator* spriteAnimator = object->GetComponent<SpriteAnimator>();
	if (spriteAnimator != nullptr)
	{
	    json comp;
	    comp["Cols"] = spriteAnimator->cols;
	    comp["Rows"] = spriteAnimator->rows;
	    json animsArray = json::array();
	    for (const auto& [name, clip] : spriteAnimator->animations)
	    {
		json clipJson;
		clipJson["Name"] = clip.name;
		clipJson["StartFrame"] = clip.startFrame;
		clipJson["FrameCount"] = clip.frameCount;
		clipJson["Speed"] = clip.speed;
		clipJson["Loop"] = clip.loop;
		animsArray.push_back(clipJson);
	    }
	    comp["Animations"] = animsArray;
	    objectData["Components"]["SpriteAnimator"] = comp;
	}
	CameraDirector* cameraDirector = object->GetComponent<CameraDirector>();
	if (cameraDirector != nullptr)
	{
	    json comp;
	    comp["Offset"] = Vec3ToJson(cameraDirector->offset);
	    comp["SmoothedSpeed"] = cameraDirector->smoothSpeed;
	    objectData["Components"]["CameraDirector"] = comp;
	}
	Terrain* terrain = object->GetComponent<Terrain>();
	if (terrain != nullptr)
	{
	    json comp;
	    comp["Width"] = terrain->width;
	    comp["Depth"] = terrain->depth;
	    comp["TileSize"] = terrain->tileSize;

	    comp["HeightMap"] = terrain->heightMap;
	    comp["TileMap"] = terrain->tileMap;
	    comp["AtlasCols"] = terrain->atlasCols;
	    comp["AtlasRows"] = terrain->atlasRows;
	    comp["AtlasTexture"] =
		terrain->atlasTextureName.empty() ? "atlas.png" : terrain->atlasTextureName;

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
			std::string normalName = mData.value("NormalFile", "None");
			std::string specularName = mData.value("SpecularFile", "None");

			uint32_t meshID =
			    resourceManager->CreateMesh(meshName, Paths::Models / meshName);
			std::filesystem::path texturePath = Paths::Textures / textureName;
			uint32_t textureID = 0;
			if (std::filesystem::exists(texturePath))
			    textureID = resourceManager->CreateTexture(textureName, texturePath);

			uint32_t normalID = 0;
			bool hasNormal = false;
			if (normalName != "None" && !normalName.empty())
			{
			    std::filesystem::path normalPath = Paths::Textures / normalName;
			    normalID = resourceManager->CreateTexture(normalName, normalPath);
			    hasNormal = true;
			}
			uint32_t specularID = 0;
			bool hasSpecular = false;
			if (specularName != "None" && !specularName.empty())
			{
			    std::filesystem::path specPath = Paths::Textures / specularName;
			    if (std::filesystem::exists(specPath))
			    {
				specularID = resourceManager->CreateTexture(specularName, specPath);
				hasSpecular = true;
			    }
			}

			std::string materialName = "Mat_" + textureName;
			uint32_t materialID =
			    resourceManager->CreateMaterial(materialName, textureID);
			if (hasNormal)
			{
			    Material* material = resourceManager->GetMaterial(materialID);
			    if (material)
				material->normalMap = resourceManager->GetTexture(normalID);
			}
			if (hasSpecular)
			{
			    Material* material = resourceManager->GetMaterial(materialID);
			    if (material)
				material->specularMap = resourceManager->GetTexture(specularID);
			}
			MeshRenderer* meshRenderer = newObject->AddComponent<MeshRenderer>();
			meshRenderer->SetMesh(meshID);
			meshRenderer->SetMaterial(materialID);

			meshRenderer->textureTiling = JsonToVec2(mData["Tiling"]);
			meshRenderer->textureOffset = JsonToVec2(mData["Offset"]);
			meshRenderer->colorTint = JsonToVec3(mData["Color"], glm::vec3(1.0f));
			meshRenderer->castShadows = mData.value("CastShadows", true);
			meshRenderer->receiveShadows = mData.value("ReceiveShadows", true);
		    }
		    if (comps.contains("PlayerController"))
		    {
			const auto& PlayerControllerData = comps["PlayerController"];
			PlayerController* playerController =
			    newObject->AddComponent<PlayerController>();
			playerController->moveSpeed = PlayerControllerData.value("MoveSpeed", 5.0f);
		    }
		    if (comps.contains("SpriteRenderer"))
		    {
			const auto& sData = comps["SpriteRenderer"];
			SpriteRenderer* spriteRenderer = newObject->AddComponent<SpriteRenderer>();
			spriteRenderer->isBillboard = sData.value("IsBillboard", false);
			spriteRenderer->lockY = sData.value("LockY", false);
		    }
		    if (comps.contains("SpriteAnimator"))
		    {
			const auto& aData = comps["SpriteAnimator"];
			SpriteAnimator* spriteAnimator = newObject->AddComponent<SpriteAnimator>();
			int c = aData.value("Cols", 1);
			int r = aData.value("Rows", 1);
			spriteAnimator->SetupSpriteSheet(c, r);

			if (aData.contains("Animations"))
			{
			    for (const auto& animJson : aData["Animations"])
			    {
				std::string n = animJson.value("Name", "Default");
				int start = animJson.value("StartFrame", 0);
				int count = animJson.value("FrameCount", 1);
				float speed = animJson.value("Loop", true);
				bool loop = animJson.value("Loop", true);
				spriteAnimator->AddAnimation(n, start, count, speed, loop);
			    }
			}
		    }
		    if (comps.contains("CameraDirector"))
		    {
			const auto& cData = comps["CameraDirector"];
			CameraDirector* cameraDirector = newObject->AddComponent<CameraDirector>();
			cameraDirector->offset = JsonToVec3(cData["Offset"], glm::vec3(0, 10, 8));
			cameraDirector->smoothSpeed = cData.value("SmoothSpeed", 5.0f);
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
			if (tData.contains("TileMap"))
			{
			    terrain->tileMap = tData["TileMap"].get<std::vector<int>>();
			}
			terrain->atlasCols = tData.value("AtlasCols", 1);
			terrain->atlasRows = tData.value("AtlasRows", 1);
			terrain->atlasTextureName = tData.value("AtlasTexture", "atlas.png");
			uint32_t atlasID = resourceManager->CreateTexture(
			    terrain->atlasTextureName, Paths::Tilemaps / terrain->atlasTextureName);

			if (auto rend = newObject->GetComponent<MeshRenderer>())
			{
			    if (auto mat = resourceManager->GetMaterial(rend->materialID))
			    {
				mat->diffuseMap = resourceManager->GetTexture(atlasID);
				mat->colorTint = glm::vec3(1.0f);
			    }
			}

			terrain->isDirty = true;
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