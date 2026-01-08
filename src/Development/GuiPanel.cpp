#include "GuiPanel.h"
#include <iostream>
#include "Development/Terminal.h"
#include <filesystem>
#include "Components/Light.h"
#include "Components/MeshRenderer.h"
#include "Resources/ResourceManager.h"
#include "Graphics/Material.h"

void GuiPanels::DrawHierarchy(Scene* scene, int& selectedIndex)
{
    float windowHeight = ImGui::GetIO().DisplaySize.y;
    ImGui::SetNextWindowPos(ImVec2(0, 80.0));
    ImGui::SetNextWindowSize(ImVec2(250, windowHeight - 80.0f));

    ImGui::Begin("Hierarchy", nullptr,
		 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    ImGui::Text("Scene Hierarchy");
    ImGui::Separator();
    for (int i = 0; i < scene->gameObjects.size(); i++)
    {
	GameObject* object = scene->gameObjects[i];
	if (!object)
	    continue;
	std::string label = object->name.empty() ? "Object " + std::to_string(i) : object->name;
	label += "##" + std::to_string(i);

	bool isSelected = (selectedIndex == i);
	if (ImGui::Selectable(label.c_str(), isSelected))
	{
	    selectedIndex = i;
	}
    }
    ImGui::End();
}
void GuiPanels::DrawInspector(Scene* scene, int& selectedIndex, ResourceManager* resourceManager,
			      std::vector<std::string>& meshList, int& selectedMeshIndex,
			      std::vector<std::string>& textureList, int& selectedTextureIndex)
{
    float windowWidth = ImGui::GetIO().DisplaySize.x;
    float windowHeight = ImGui::GetIO().DisplaySize.y;
    float sidebarWidth = 300.0f;
    ImGui::SetNextWindowPos(ImVec2(windowWidth - sidebarWidth, 80.0f));
    ImGui::SetNextWindowSize(ImVec2(sidebarWidth, windowHeight - 80.0f));

    ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    if (selectedIndex >= 0 && selectedIndex < scene->gameObjects.size())
    {
	GameObject* object = scene->gameObjects[selectedIndex];
	if (object)
	{
	    ImGui::Text("ID: %d | Name: %s", selectedIndex, object->name.c_str());
	    ImGui::Separator();

	    DrawTransform(object);
	    DrawMeshRenderer(object, resourceManager, meshList, selectedMeshIndex, textureList,
			     selectedTextureIndex);
	    DrawLight(object);

	    ImGui::Separator();
	    if (ImGui::Button("Delete Object", ImVec2(-1, 0)))
	    {
		scene->RemoveGameObject(object);
		selectedIndex = -1;
	    }
	}
    } else
    {
	ImGui::Text("No object selected");
    }
    ImGui::End();
}
void GuiPanels::DrawTransform(GameObject* object)
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

void GuiPanels::DrawMeshRenderer(GameObject* object, ResourceManager* resourceManager,
				 std::vector<std::string>& meshList, int& meshIndex,
				 std::vector<std::string>& textureList, int& textureIndex)
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
	if (!meshList.empty())
	{
	    bool comboOpen = ImGui::Combo(
		"Mesh Asset", &meshIndex,
		[](void* data, int index, const char** out_text) {
		    auto* vec = static_cast<std::vector<std::string>*>(data);
		    if (index < 0 || index >= (int)vec->size())
			return false;
		    *out_text = vec->at(index).c_str();
		    return true;
		},
		&meshList, (int)meshList.size());
	    if (comboOpen)
	    {
		std::filesystem::path CurrentPath = ResourceManager::FolderFinder("assets");
		std::filesystem::path path = CurrentPath / "assets/models" / meshList[meshIndex];
		uint32_t newId = resourceManager->CreateMesh(meshList[meshIndex], path);
		meshRenderer->SetMesh(newId);
		object->name = meshList[meshIndex];
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

	    if (!textureList.empty())
	    {
		bool textureComboOpen = ImGui::Combo(
		    "Diffuse Map", &textureIndex,
		    [](void* data, int index, const char** out_text) {
			std::vector<std::string>* vector =
			    static_cast<std::vector<std::string>*>(data);
			if (index < 0 || index >= (int)vector->size())
			    return false;
			*out_text = vector->at(index).c_str();
			return true;
		    },
		    &textureList, (int)textureList.size());

		if (textureComboOpen)
		{
		    std::filesystem::path CurrentPath = ResourceManager::FolderFinder("assets");
		    std::filesystem::path path =
			CurrentPath / "assets/textures" / textureList[textureIndex];
		    uint32_t newTextureId =
			resourceManager->CreateTexture(textureList[textureIndex], path);

		    std::string newMatName =
			"Mat_" + textureList[textureIndex] + "_" + std::to_string(rand());
		    uint32_t newMatId = resourceManager->CreateMaterial(newMatName, newTextureId);
		    meshRenderer->SetMaterial(newMatId);
		}
	    }
	}
    }
}

void GuiPanels::DrawLight(GameObject* object)
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
