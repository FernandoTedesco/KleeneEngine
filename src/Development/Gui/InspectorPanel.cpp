#include "InspectorPanel.h"
#include "Components/MeshRenderer.h"
#include "Components/Light.h"
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
	    DrawMeshRenderer(object, resourceManager);
	    DrawLight(object);

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
    bool hasMesh = (meshRenderer != nullptr);
    if (ImGui::Checkbox("Mesh Renderer", &hasMesh))
    {
	if (hasMesh && !meshRenderer)
	    object->AddComponent<MeshRenderer>();
    }
    if (meshRenderer)
    {
	if (!availableMeshes.empty())
	{
	    if (ImGui::Combo(
		    "Mesh Asset", &selectedMeshIndex,
		    [](void* data, int index, const char** out_text) {
			auto* vec = static_cast<std::vector<std::string>*>(data);
			if (index < 0 || index >= vec->size())
			    return false;
			*out_text = vec->at(index).c_str();
			return true;
		    },
		    &availableMeshes, (int)availableMeshes.size()))
	    {
	    }
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

	    if (!availableTextures.empty())
	    {
		bool textureComboOpen = ImGui::Combo(
		    "Diffuse Map", &selectedTextureIndex,
		    [](void* data, int index, const char** out_text) {
			std::vector<std::string>* vector =
			    static_cast<std::vector<std::string>*>(data);
			if (index < 0 || index >= (int)vector->size())
			    return false;
			*out_text = vector->at(index).c_str();
			return true;
		    },
		    &selectedTextureIndex, (int)availableTextures.size());

		if (textureComboOpen)
		{
		    std::filesystem::path CurrentPath = ResourceManager::FolderFinder("assets");
		    std::filesystem::path path =
			CurrentPath / "assets/textures" / availableTextures[selectedTextureIndex];
		    uint32_t newTextureId = resourceManager->CreateTexture(
			availableTextures[selectedTextureIndex], path);

		    std::string newMatName = "Mat_" + availableTextures[selectedTextureIndex] +
					     "_" + std::to_string(rand());
		    uint32_t newMatId = resourceManager->CreateMaterial(newMatName, newTextureId);
		    meshRenderer->SetMaterial(newMatId);
		}
	    }
	}
    }
}

void InspectorPanel::DrawLight(GameObject* object)
{
    Light* light = object->GetComponent<Light>();
    bool hasLight = (light != nullptr);

    if (ImGui::Checkbox("Light Component", &hasLight))
    {
	if (light)
	{
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