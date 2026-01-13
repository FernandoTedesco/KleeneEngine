#include "InspectorPanel.h"

#include "Components/MeshRenderer.h"
#include "Components/PlayerController.h"
#include "Components/Terrain.h"
#include "Components/SpriteRenderer.h"
#include "Components/Light.h"
#include "Components/ParticleSystem.h"
#include "Components/SpriteRenderer.h"
#include "Components/SpriteAnimator.h"
#include "Components/CameraDirector.h"

#include "Core/Paths.h"
#include "Graphics/Material.h"
#include <iostream>
#include <cstring>

void InspectorPanel::Draw(Scene* scene, int& selectedEntityIndex, ResourceManager* resourceManager)
{
    if (!listLoaded)
    {
	RefreshAssetLists();
	listLoaded = true;
    }
    float windowWidth = ImGui::GetIO().DisplaySize.x;
    float windowHeight = ImGui::GetIO().DisplaySize.y;
    float sidebarWidth = 200.0f;
    float barHeight = 80.0f;
    ImGui::SetNextWindowPos(ImVec2(windowWidth - sidebarWidth, barHeight));
    ImGui::SetNextWindowSize(ImVec2(sidebarWidth, windowHeight - barHeight));
    this->Stylize();
    ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    if (selectedEntityIndex >= 0 && selectedEntityIndex < scene->gameObjects.size())
    {
	GameObject* object = scene->gameObjects[selectedEntityIndex];
	if (object)
	{
	    ImGui::Text("ID: %d", selectedEntityIndex);
	    ImGui::SameLine();

	    char nameBuffer[64];
	    strncpy_s(nameBuffer, object->name.c_str(), sizeof(nameBuffer));
	    nameBuffer[sizeof(nameBuffer) - 1] = 0;
	    if (ImGui::InputText("Name", nameBuffer, 64))
	    {
		object->name = std::string(nameBuffer);
	    }
	    ImGui::Separator();

	    DrawTransform(object);
	    DrawTerrain(object);
	    DrawMeshRenderer(object, resourceManager);
	    DrawLight(object);
	    DrawParticleSystem(object);
	    DrawPlayerController(object);
	    DrawSpriteRenderer(object);
	    DrawSpriteAnimator(object);
	    DrawCameraDirector(object);

	    ImGui::Separator();
	    DrawAddComponentButton(object, resourceManager);
	    ImGui::Separator();
	    if (ImGui::Button("Delete Object", ImVec2(-1, 0)))
	    {
		scene->RemoveGameObject(object);
		selectedEntityIndex = -1;
	    }
	}
    } else
    {
	ImGui::Text("No object selected");
    }

    ImGui::End();
    ImGui::PopStyleColor(6);
}

void InspectorPanel::DrawTransform(GameObject* object)
{
    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
    {
	glm::vec3 position = object->position;
	glm::vec3 scale = object->scale;
	glm::vec3 rotation = object->rotation;

	if (ImGui::DragFloat3("Position", &position[0], 0.1f))
	    object->SetPosition(position);
	if (ImGui::DragFloat3("Scale", &scale[0], 0.05f))
	    object->SetScale(scale);
	if (ImGui::DragFloat3("Rotation", &rotation[0], 1.0f))
	    object->SetRotation(rotation);
    }
}

void InspectorPanel::DrawMeshRenderer(GameObject* object, ResourceManager* resourceManager)
{
    MeshRenderer* meshRenderer = object->GetComponent<MeshRenderer>();
    if (!meshRenderer)
	return;
    if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen))
    {
	if (ImGui::BeginPopupContextItem("MeshRendererCtx"))
	{
	    if (ImGui::MenuItem("Remove Component"))
	    {
		object->RemoveComponent<MeshRenderer>();
		ImGui::EndPopup();
		return;
	    }
	    ImGui::EndPopup();
	}
	bool controlledByTerrain = (object->GetComponent<Terrain>() != nullptr);
	if (controlledByTerrain)
	{
	    ImGui::TextDisabled("Mesh Source: Terrain");
	    ImGui::SameLine();
	    ImGui::TextDisabled("(Locked)");
	}
	if (!availableMeshes.empty() && !controlledByTerrain)
	{
	    if (ImGui::Combo(
		    "Mesh Asset", &selectedMeshIndex,
		    [](void* data, int index, const char** out_text) {
			auto* vec = static_cast<std::vector<std::string>*>(data);
			if (!vec || index < 0 || index >= vec->size())
			    return false;
			if (index < 0 || index >= vec->size())
			    return false;
			*out_text = vec->at(index).c_str();
			return true;
		    },
		    &availableMeshes, (int)availableMeshes.size()))
	    {
		std::filesystem::path root = ResourceManager::FolderFinder("assets");
		std::filesystem::path path =
		    root / "assets/models" / availableMeshes[selectedMeshIndex];

		uint32_t newId =
		    resourceManager->CreateMesh(availableMeshes[selectedMeshIndex], path.string());
		;
		meshRenderer->SetMesh(newId);
		if (object->name.empty() || object->name.find("Object") != std::string::npos)
		    object->name = availableMeshes[selectedMeshIndex];
	    }
	}
	Material* material = resourceManager->GetMaterial(meshRenderer->materialID);
	if (material)
	{
	    ImGui::Separator();
	    ImGui::Text("Material Propertiies");
	    ImGui::ColorEdit3("Tint", &meshRenderer->colorTint[0]);
	    ImGui::DragFloat2("Tiling", &meshRenderer->textureTiling[0], 0.1f);
	    ImGui::DragFloat2("Offset", &meshRenderer->textureOffset[0], 0.01f);

	    ImGui::Spacing();
	    ImGui::DragFloat("Specular", &material->specular, 0.05f, 0.0f, 1.0f);
	    ImGui::DragFloat("Shininess", &material->shininess, 1.0f, 2.0f, 256.0f);
	    ImGui::Separator();
	    ImGui::Text("Textures");
	    if (!availableTextures.empty())
	    {
		Texture* newDiffuse =
		    DrawTextureSelector("Diffuse Map", material->diffuseMap, resourceManager);
		if (newDiffuse != material->diffuseMap)
		{
		    material->diffuseMap = newDiffuse;
		}
		Texture* newNormal =
		    DrawTextureSelector("Normal Map", material->normalMap, resourceManager);
		if (newNormal != material->normalMap)
		{
		    material->normalMap = newNormal;
		}
		Texture* newSpecular = DrawTextureSelector("Specular/Roughness",
							   material->specularMap, resourceManager);
		if (newSpecular != material->specularMap)
		{
		    material->specularMap = newSpecular;
		}
	    }
	}
    }
}

void InspectorPanel::DrawTerrain(GameObject* object)
{
    Terrain* terrain = object->GetComponent<Terrain>();
    if (!terrain)
	return;

    if (!terrain)
	return;
    if (ImGui::CollapsingHeader("Terrain Generator", ImGuiTreeNodeFlags_DefaultOpen))
    {
	if (ImGui::BeginPopupContextItem("TerrainCtx"))
	{
	    if (ImGui::MenuItem("Remove Component"))
	    {
		object->RemoveComponent<Terrain>();
		ImGui::EndPopup();
		return;
	    }
	    ImGui::EndPopup();
	}

	int w = terrain->width;
	int d = terrain->depth;
	float s = terrain->tileSize;

	bool changed = false;
	if (ImGui::DragInt("Width", &w, 1, 2, 256))
	    changed = true;
	if (ImGui::DragInt("Depth", &d, 1, 2, 256))
	    changed = true;
	if (ImGui::DragFloat("Tile Size", &s, 0.1, 0.1, 100.0f))
	    changed = true;
	if (changed)
	{
	    terrain->InitializeGrid(w, d, s);
	}
	if (ImGui::Button("Force Rebuild Mesh"))
	{
	    terrain->isDirty = true;
	}
	ImGui::Text("Vertices: %d", (w * d) * 4);
    }
}

void InspectorPanel::DrawAddComponentButton(GameObject* object, ResourceManager* resourceManager)
{
    if (ImGui::Button("Add Component", ImVec2(-1, 0)))
	ImGui::OpenPopup("AddComponentPopup");
    if (ImGui::BeginPopup("AddComponentPopup"))
    {
	if (!object->GetComponent<MeshRenderer>() && ImGui::MenuItem("Mesh Renderer"))
	{
	    object->AddComponent<MeshRenderer>();
	}
	if (!object->GetComponent<Light>() && ImGui::MenuItem("Light"))
	{
	    object->AddComponent<Light>();
	}
	if (!object->GetComponent<CameraDirector>() && ImGui::MenuItem("CameraDirector"))
	{
	    object->AddComponent<CameraDirector>();
	}
	if (!object->GetComponent<Terrain>() && ImGui::MenuItem("Terrain"))
	{
	    Terrain* terrain = object->AddComponent<Terrain>();
	    terrain->SetResourceManager(resourceManager);
	}
	if (!object->GetComponent<SpriteRenderer>() && ImGui::MenuItem("SpriteRenderer"))
	{
	    object->AddComponent<SpriteRenderer>();
	}
	if (!object->GetComponent<SpriteAnimator>() && ImGui::MenuItem("SpriteAnimator"))
	{
	    object->AddComponent<SpriteAnimator>();
	}
	if (!object->GetComponent<PlayerController>() && ImGui::MenuItem("PlayerController"))
	{
	    object->AddComponent<PlayerController>();
	}
	if (!object->GetComponent<ParticleSystem>() && ImGui::MenuItem("ParticleSystem"))
	{
	    object->AddComponent<ParticleSystem>();
	}
	ImGui::EndPopup();
    }
}
void InspectorPanel::DrawLight(GameObject* object)
{
    Light* light = object->GetComponent<Light>();
    if (!light)
	return;

    if (ImGui::CollapsingHeader("Light Component", ImGuiTreeNodeFlags_DefaultOpen))
    {
	if (ImGui::BeginPopupContextItem("LightCtx"))
	{
	    if (ImGui::MenuItem("Remove Component"))
	    {
		object->RemoveComponent<Light>();
		ImGui::EndPopup();
		return;
	    }
	    ImGui::EndPopup();
	}

	const char* types[] = {"Directional", "Point", "Spot"};
	int currentType = (int)light->type;
	if (ImGui::Combo("Type", &currentType, types, 3))
	{
	    light->type = (LightType)currentType;
	}
	ImGui::ColorEdit3("Color", &light->color[0]);
	ImGui::DragFloat("Intensity", &light->intensity, 0.1f, 0.0f, 100.0f);

	if (light->type != LightType::Directional)
	{
	    ImGui::Text("Attenuation");
	    ImGui::DragFloat("Linear", &light->linear, 0.001f, 0.0f, 1.0f, "%.4f");
	    ImGui::DragFloat("Quadratic", &light->quadratic, 0.001f, 0.0f, 1.0f, "%.4f");
	}
	if (light->type == LightType::Spot)
	{
	    ImGui::Text("Spot Angles");
	    ImGui::DragFloat("Inner CutOff", &light->cutOff, 0.5f, 0.0f, 180.0f);
	    ImGui::DragFloat("Outer CutOff", &light->outerCutOff, 0.5f, 0.0f, 180.0f);
	}
    }
}

std::vector<std::string> InspectorPanel::ScanDirectory(const std::filesystem::path& directoryPath)
{
    std::vector<std::string> files;

    if (!std::filesystem::exists(directoryPath))
    {
	return files;
    }
    for (const std::filesystem::directory_entry& entry :
	 std::filesystem::directory_iterator(directoryPath))
    {
	if (entry.is_regular_file())
	{
	    files.push_back(entry.path().filename().string());
	}
    }
    return files;
}

void InspectorPanel::RefreshAssetLists()
{
    std::filesystem::path currentPath = ResourceManager::FolderFinder("assets");
    availableMeshes = ScanDirectory(currentPath / "assets/models");
    availableTextures = ScanDirectory(currentPath / "assets/textures");
}

void InspectorPanel::Stylize()
{
    ImVec4 fixedColor = ImVec4(0.05f, 0.2f, 0.05f, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_TitleBg, fixedColor);
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, fixedColor);

    ImGui::PushStyleColor(ImGuiCol_Header, fixedColor);
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, fixedColor);

    ImGui::PushStyleColor(ImGuiCol_Button, fixedColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, fixedColor);
}

void InspectorPanel::DrawParticleSystem(GameObject* object)
{
    ParticleSystem* particleSystem = object->GetComponent<ParticleSystem>();
    if (!particleSystem)
	return;
    if (ImGui::CollapsingHeader("Particle System", ImGuiTreeNodeFlags_DefaultOpen))
    {
	if (ImGui::BeginPopupContextItem("ParticleSystemCtx"))
	{
	    if (ImGui::MenuItem("Remove Component"))
	    {
		object->RemoveComponent<ParticleSystem>();
		ImGui::EndPopup();
		return;
	    }
	    ImGui::EndPopup();
	}
	ImGui::DragFloat("Spawn Rate", &particleSystem->spawnRate, 0.5f, 0.0f, 500.0f);
	ImGui::DragFloat3("Offset", &particleSystem->offset[0], 0.1f);
	ImGui::Text("Active particles managed by the Global System");
    }
}

Texture* InspectorPanel::DrawTextureSelector(const char* label, Texture* currentTexture,
					     ResourceManager* resourceManager)
{
    int selectedIndex = -1;
    std::string currentTextureName = "None";
    if (currentTexture)
    {
	for (size_t i = 0; i < resourceManager->textureVector.size(); i++)
	{
	    if (resourceManager->textureVector[i] == currentTexture)
	    {
		if (i < resourceManager->textureNames.size())
		    currentTextureName = resourceManager->textureNames[i];
		break;
	    }
	}
	for (int i = 0; i < availableTextures.size(); i++)
	{
	    if (availableTextures[i] == currentTextureName)
	    {
		selectedIndex = i;
		break;
	    }
	}
    }
    Texture* resultTexture = currentTexture;
    ImGui::PushID(label);
    if (ImGui::Combo(
	    label, &selectedIndex,
	    [](void* data, int index, const char** out_text) {
		auto* vec = static_cast<std::vector<std::string>*>(data);
		if (index < 0 || index >= vec->size())
		    return false;
		*out_text = vec->at(index).c_str();
		return true;
	    },
	    &availableTextures, ((int)availableTextures.size())))
    {
	if (selectedIndex >= 0 && selectedIndex < availableTextures.size())
	{
	    std::filesystem::path path = Paths::Textures / availableTextures[selectedIndex];
	    uint32_t newTextureID =
		resourceManager->CreateTexture(availableTextures[selectedIndex], path);
	    resultTexture = resourceManager->GetTexture(newTextureID);
	}
    }
    if (resultTexture)
    {
	ImGui::SameLine();
	if (ImGui::Button("X"))
	{
	    resultTexture = nullptr;
	}
    }
    ImGui::PopID();
    return resultTexture;
}

void InspectorPanel::DrawPlayerController(GameObject* object)
{
    PlayerController* playerController = object->GetComponent<PlayerController>();
    if (!playerController)
	return;
    if (ImGui::CollapsingHeader("Player Controller", ImGuiTreeNodeFlags_DefaultOpen))
    {
	if (ImGui::BeginPopupContextItem("PlayerControllerCtx"))
	{
	    if (ImGui::MenuItem("Remove Component"))
	    {
		object->RemoveComponent<PlayerController>();
		ImGui::EndPopup();
		return;
	    }
	    ImGui::EndPopup();
	}
	ImGui::DragFloat("Move Speed", &playerController->moveSpeed, 0.1f, 0.0f, 100.0f);
	ImGui::TextDisabled("Status:%s", (playerController->moveSpeed > 0 ? "Active" : "Stopped"));
    }
}

void InspectorPanel::DrawSpriteRenderer(GameObject* object)
{
    SpriteRenderer* spriteRenderer = object->GetComponent<SpriteRenderer>();
    if (!spriteRenderer)
	return;
    if (ImGui::CollapsingHeader("Sprite Renderer", ImGuiTreeNodeFlags_DefaultOpen))
    {
	if (ImGui::BeginPopupContextItem("SpriteRendererCtx"))
	{
	    if (ImGui::MenuItem("Remove Component"))
	    {
		object->RemoveComponent<SpriteRenderer>();
		ImGui::EndPopup();
		return;
	    }
	    ImGui::EndPopup();
	}
	ImGui::Checkbox("Billboard Mode", &spriteRenderer->isBillboard);
	if (spriteRenderer->targetCamera == nullptr)
	{
	    ImGui::TextColored(ImVec4(1, 0, 0, 1), "No Camera Linked!");
	}
    }
}

void InspectorPanel::DrawSpriteAnimator(GameObject* object)
{
    SpriteAnimator* animator = object->GetComponent<SpriteAnimator>();
    if (!animator)
	return;

    if (ImGui::CollapsingHeader("Sprite Animator", ImGuiTreeNodeFlags_DefaultOpen))
    {
	if (ImGui::BeginPopupContextItem("SpriteAnimCtx"))
	{
	    if (ImGui::MenuItem("Remove Component"))
	    {
		object->RemoveComponent<SpriteAnimator>();
		ImGui::EndPopup();
		return;
	    }
	    ImGui::EndPopup();
	}
	ImGui::Text("Sheet Layout");
	bool gridChanged = false;
	if (ImGui::InputInt("Columns", &animator->cols))
	    gridChanged = true;
	if (ImGui::InputInt("Rows", &animator->rows))
	    gridChanged = true;

	if (gridChanged)
	{
	    if (animator->cols < 1)
		animator->cols = 1;
	    if (animator->rows < 1)
		animator->rows = 1;
	    animator->SetupSpriteSheet(animator->cols, animator->rows);
	}
	ImGui::Separator();
	ImGui::Text("Animations List");

	static char newAnimatorName[32] = "New Animation";
	ImGui::InputText("##NewName", newAnimatorName, 32);
	ImGui::SameLine();
	if (ImGui::Button("Create Clip"))
	{
	    animator->AddAnimation(newAnimatorName, 0, 1, 0.1f, true);
	}
	ImGui::BeginChild("AnimatorList", ImVec2(0, 150), true);
	std::string animationToRemove = "";
	for (auto& [name, clip] : animator->animations)
	{
	    ImGui::PushID(name.c_str());
	    if (ImGui::TreeNode(name.c_str()))
	    {
		ImGui::DragInt("Start Frame", &clip.startFrame, 1, 0,
			       (animator->cols * animator->rows) - 1);
		ImGui::DragInt("Count", &clip.frameCount, 1, 1, 100);
		ImGui::DragFloat("Speed", &clip.speed, 0.01f, 0.001f, 5.0f);
		ImGui::Checkbox("Loop", &clip.loop);

		if (ImGui::Button("Play Now"))
		{
		    animator->Play(name);
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete", ImVec2(60, 0)))
		{
		    animationToRemove = name;
		}
		ImGui::TreePop();
	    }
	    ImGui::PopID();
	}
	ImGui::EndChild();
	if (!animationToRemove.empty())
	{
	    animator->animations.erase(animationToRemove);
	}
	ImGui::TextDisabled("Current %s | Frame: %d", animator->currentAnimationName.c_str(),
			    animator->currentFrameIndex);
    }
}

void InspectorPanel::DrawCameraDirector(GameObject* object)
{
    CameraDirector* cameraDirector = object->GetComponent<CameraDirector>();
    if (!cameraDirector)
	return;

    if (ImGui::CollapsingHeader("Camera Director", ImGuiTreeNodeFlags_DefaultOpen))
    {

	if (ImGui::BeginPopupContextItem("CamDirCtx"))
	{
	    if (ImGui::MenuItem("Remove Component"))
	    {
		object->RemoveComponent<CameraDirector>();
		ImGui::EndPopup();
		return;
	    }
	    ImGui::EndPopup();
	}
	ImGui::DragFloat3("CamOffset", &cameraDirector->offset[0], 0.1f);
	ImGui::DragFloat("Smoothness", &cameraDirector->smoothSpeed, 0.1f, 0.1f, 20.0f);

	if (cameraDirector->globalCameraRef == nullptr)
	{
	    ImGui::TextColored(ImVec4(1, 0, 0, 1), "WAITING FOR CAMERA LINK...");
	} else
	{
	    ImGui::TextColored(ImVec4(0, 1, 0, 1), "Camera Linked Active");
	}
    }
}
