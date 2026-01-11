#include "EditorTools.h"
#include "Scenes/Scene.h"
#include "Scenes/GameObject.h"
#include "Core/Input.h"
#include "imgui.h"
#include "Utils/Math.h"
#include <iostream>
#include <filesystem>
#include <algorithm>
#include "Terminal.h"
#include "Graphics/Mesh.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Components/MeshRenderer.h"
#include "Components/Terrain.h"
#include "Resources/ResourceManager.h"
#include "Core/Paths.h"
EditorTools::EditorTools()
    : isDragging(false), currentAxis(GizmoAxis::NONE), draggingStartMousePosition(0.0f),
      draggingStartPosition(0.0f), draggingStartScale(1.0f), draggingStartRotation(0.0f)
{
    this->gizmo = new Gizmo();
}
EditorTools::~EditorTools()
{
    delete this->gizmo;
}

void EditorTools::UpdateTranslation(Scene* scene, int& selectedIndex, glm::vec3 rayOrigin,
				    glm::vec3 rayDirection, ResourceManager* resourceManager)
{
    bool handledByGizmo = false;
    GameObject* object = nullptr;
    if (selectedIndex >= 0 && selectedIndex < scene->gameObjects.size())
    {
	object = scene->gameObjects[selectedIndex];
	if (ImGui::IsMouseClicked(0) && !ImGui::GetIO().WantCaptureMouse)
	{
	    currentAxis = gizmo->CheckHover(rayOrigin, rayDirection, object->position, 1.0f);
	    if (currentAxis != GizmoAxis::NONE)
	    {
		isDragging = true;
		draggingStartPosition = object->position;
		ImVec2 mPos = ImGui::GetMousePos();
		draggingStartMousePosition = glm::vec2(mPos.x, mPos.y);
		handledByGizmo = true;
	    }
	}
    }
    if (!handledByGizmo && !isDragging && ImGui::IsMouseClicked(0) &&
	!ImGui::GetIO().WantCaptureMouse)
    {
	SelectObject(scene, selectedIndex, rayOrigin, rayDirection, resourceManager);
	if (selectedIndex != -1)
	{
	    object = scene->gameObjects[selectedIndex];
	} else
	{
	    object = nullptr;
	}
    }
    if (object && isDragging && ImGui::IsMouseDown(0))
    {
	ImVec2 currentMouse = ImGui::GetMousePos();
	float deltaX = currentMouse.x - draggingStartMousePosition.x;
	float deltaY = currentMouse.y - draggingStartMousePosition.y;
	float sensitivity = 0.02f;

	glm::vec3 newPos = draggingStartPosition;
	if (currentAxis == GizmoAxis::X)
	    newPos.x += deltaX * sensitivity;
	if (currentAxis == GizmoAxis::Y)
	    newPos.y -= deltaY * sensitivity;
	if (currentAxis == GizmoAxis::Z)
	    newPos.z += deltaY * sensitivity;

	if (ImGui::GetIO().KeyCtrl)
	{
	    float gridSnap = 1.0f;
	    if (currentAxis == GizmoAxis::X)
		newPos.x = std::floor(newPos.x / gridSnap) + 0.5f;
	    if (currentAxis == GizmoAxis::Y)
		newPos.y = std::floor(newPos.y / gridSnap);
	    if (currentAxis == GizmoAxis::Z)
		newPos.z = std::floor(newPos.z / gridSnap) + 0.5f;
	}

	object->SetPosition(newPos);
	object->position = newPos;
    }
    if (ImGui::IsMouseReleased(0))
    {
	isDragging = false;
	currentAxis = GizmoAxis::NONE;
    }
}

void EditorTools::UpdateRotation(Scene* scene, int& selectedIndex, glm::vec3 rayOrigin,
				 glm::vec3 rayDirection, ResourceManager* resourceManager)
{
    bool handledByGizmo = false;
    GameObject* object = nullptr;
    if (selectedIndex >= 0 && selectedIndex < scene->gameObjects.size())
    {
	object = scene->gameObjects[selectedIndex];
	if (ImGui::IsMouseClicked(0) && !ImGui::GetIO().WantCaptureMouse)
	{
	    currentAxis = gizmo->CheckHover(rayOrigin, rayDirection, object->position, 1.0f);
	    if (currentAxis != GizmoAxis::NONE)
	    {
		isDragging = true;
		draggingStartPosition = object->rotation;
		ImVec2 mPos = ImGui::GetMousePos();
		draggingStartMousePosition = glm::vec2(mPos.x, mPos.y);
		handledByGizmo = true;
	    }
	}
    }
    if (!handledByGizmo && !isDragging && ImGui::IsMouseClicked(0) &&
	!ImGui::GetIO().WantCaptureMouse)
    {
	SelectObject(scene, selectedIndex, rayOrigin, rayDirection, resourceManager);
	if (selectedIndex != -1)
	{
	    object = scene->gameObjects[selectedIndex];
	} else
	{
	    object = nullptr;
	}
    }
    if (object && isDragging && ImGui::IsMouseDown(0))
    {
	ImVec2 currentMouse = ImGui::GetMousePos();
	float deltaX = currentMouse.x - draggingStartMousePosition.x;
	float deltaY = currentMouse.y - draggingStartMousePosition.y;
	float sensitivity = 0.5f;

	glm::vec3 newRot = draggingStartPosition;
	if (currentAxis == GizmoAxis::X)
	    newRot.x -= deltaY * sensitivity;
	if (currentAxis == GizmoAxis::Y)
	    newRot.y -= deltaX * sensitivity;
	if (currentAxis == GizmoAxis::Z)
	    newRot.z -= deltaX * sensitivity;

	if (ImGui::GetIO().KeyCtrl)
	{
	    float angleSnap = 15.0f;
	    if (currentAxis == GizmoAxis::X)
		newRot.x = std::floor(newRot.x / angleSnap) * angleSnap;
	    if (currentAxis == GizmoAxis::Y)
		newRot.y = std::floor(newRot.y / angleSnap) * angleSnap;
	    if (currentAxis == GizmoAxis::Z)
		newRot.z = std::floor(newRot.z / angleSnap) * angleSnap;
	}

	object->SetRotation(newRot);
	object->rotation = newRot;
    }
    if (ImGui::IsMouseReleased(0))
    {
	isDragging = false;
	currentAxis = GizmoAxis::NONE;
    }
}

void EditorTools::UpdateScale(Scene* scene, int& selectedIndex, glm::vec3 rayOrigin,
			      glm::vec3 rayDirection, ResourceManager* resourceManager)
{
    bool handledByGizmo = false;
    GameObject* object = nullptr;
    if (selectedIndex >= 0 && selectedIndex < scene->gameObjects.size())
    {
	object = scene->gameObjects[selectedIndex];
	if (ImGui::IsMouseClicked(0) && !ImGui::GetIO().WantCaptureMouse)
	{
	    currentAxis = gizmo->CheckHover(rayOrigin, rayDirection, object->position, 1.0f);
	    if (currentAxis != GizmoAxis::NONE)
	    {
		isDragging = true;
		draggingStartPosition = object->scale;
		ImVec2 mPos = ImGui::GetMousePos();
		draggingStartMousePosition = glm::vec2(mPos.x, mPos.y);
		handledByGizmo = true;
	    }
	}
    }
    if (!handledByGizmo && !isDragging && ImGui::IsMouseClicked(0) &&
	!ImGui::GetIO().WantCaptureMouse)
    {
	SelectObject(scene, selectedIndex, rayOrigin, rayDirection, resourceManager);
	if (selectedIndex != -1)
	{
	    object = scene->gameObjects[selectedIndex];
	} else
	{
	    object = nullptr;
	}
    }
    if (object && isDragging && ImGui::IsMouseDown(0))
    {
	ImVec2 currentMouse = ImGui::GetMousePos();
	float deltaX = currentMouse.x - draggingStartMousePosition.x;
	float deltaY = currentMouse.y - draggingStartMousePosition.y;
	float sensitivity = 0.02f;

	glm::vec3 newScale = draggingStartPosition;
	if (currentAxis == GizmoAxis::X)
	    newScale.x += deltaX * sensitivity;
	if (currentAxis == GizmoAxis::Y)
	    newScale.y -= deltaY * sensitivity;
	if (currentAxis == GizmoAxis::Z)
	    newScale.z += deltaY * sensitivity;

	if (ImGui::GetIO().KeyCtrl)
	{
	    float scaleSnap = 0.5f;
	    if (currentAxis == GizmoAxis::X)
		newScale.x = std::round(newScale.x / scaleSnap) * scaleSnap;
	    if (currentAxis == GizmoAxis::Y)
		newScale.y = std::round(newScale.y / scaleSnap) * scaleSnap;
	    if (currentAxis == GizmoAxis::Z)
		newScale.z = std::round(newScale.z / scaleSnap) * scaleSnap;
	}
	newScale = glm::max(newScale, glm::vec3(0.1f));

	object->SetScale(newScale);
	object->scale = newScale;
    }
    if (ImGui::IsMouseReleased(0))
    {
	isDragging = false;
	currentAxis = GizmoAxis::NONE;
    }
}

void EditorTools::PaintTerrain(Scene* scene, int selectedIndex, glm::vec3 rayOrigin,
			       glm::vec3 rayDirection, ResourceManager* resourceManager)
{

    if (selectedIndex < 0 || selectedIndex >= scene->gameObjects.size())
	return;
    GameObject* object = scene->gameObjects[selectedIndex];
    Terrain* terrain = object->GetComponent<Terrain>();
    if (!terrain)
	return;

    glm::mat4 modelMatrix(glm::mat4(1.0f));
    modelMatrix = glm::translate(modelMatrix, glm::vec3(object->position));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(object->rotation.x), glm::vec3(1, 0, 0));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(object->rotation.y), glm::vec3(0, 1, 0));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(object->rotation.z), glm::vec3(0, 0, 1));

    glm::mat4 invModel = glm::inverse(modelMatrix);
    glm::vec4 localOriginV4 = invModel * glm::vec4(rayOrigin, 1.0f);
    glm::vec4 localDirV4 = invModel * glm::vec4(rayDirection, 0.0f);
    glm::vec3 localOrigin = glm::vec3(localOriginV4);
    glm::vec3 localDirection = glm::normalize(glm::vec3(localDirV4));
    if (std::abs(localDirection.y) < 0.0001f)
	return;

    float t = -localOrigin.y / localDirection.y;

    if (t < 0.0f)
	return;
    glm::vec3 localHit = localOrigin + (localDirection * t);
    MeshRenderer* renderer = object->GetComponent<MeshRenderer>();
    if (renderer)
    {
	Mesh* mesh = resourceManager->GetMesh(renderer->meshID);
	if (!mesh)
	    return;

	if (localHit.x >= mesh->boundsMin.x && localHit.x <= mesh->boundsMax.x &&
	    localHit.z >= mesh->boundsMin.z && localHit.z <= mesh->boundsMax.z)
	{
	    float localX = localHit.x - mesh->boundsMin.x;
	    float localZ = localHit.z - mesh->boundsMin.z;

	    float tSize = (terrain->tileSize > 0.01f) ? terrain->tileSize : 1.0f;
	    int mapTileX = (int)(localHit.x / terrain->tileSize);
	    int mapTileZ = (int)(localHit.z / terrain->tileSize);
	    if (mapTileX >= 0 && mapTileZ >= 0)
	    {
		terrain->SetTileTexture(mapTileX, mapTileZ, this->selectedTileID);
	    }
	}
    }
}

int EditorTools::RaycastScene(Scene* scene, ResourceManager* resourceManager, glm::vec3 rayOrigin,
			      glm::vec3 rayDirection)
{
    float closestDistance = FLT_MAX;
    int hitIndex = -1;
    for (int i = 0; i < scene->gameObjects.size(); i++)
    {
	GameObject* object = scene->gameObjects[i];
	if (!object)
	    continue;
	glm::vec3 aabbMin, aabbMax;
	MeshRenderer* renderer = object->GetComponent<MeshRenderer>();

	bool hasMesh = false;
	if (renderer && renderer->meshID != 0)
	{
	    Mesh* mesh = resourceManager->GetMesh(renderer->meshID);
	    if (mesh)
	    {
		aabbMin = object->position + (mesh->boundsMin * object->scale);
		aabbMax = object->position + (mesh->boundsMax * object->scale);
		hasMesh = true;
	    }
	}
	if (!hasMesh)
	{
	    glm::vec3 halfSize = object->scale * 0.5f;
	    aabbMin = object->position - halfSize;
	    aabbMax = object->position + halfSize;
	}
	float distance = 0.0f;
	if (Math::RayAABBIntersection(rayOrigin, rayDirection, aabbMin, aabbMax, distance))
	{
	    if (distance < closestDistance)
	    {
		closestDistance = distance;
		hitIndex = i;
	    }
	}
    }
    return hitIndex;
}

void EditorTools::DeleteObject(Scene* scene, int& selectedIndex, glm::vec3 rayOrigin,
			       glm::vec3 rayDirection, ResourceManager* resourceManager)
{
    if (ImGui::IsMouseClicked(0) && !ImGui::GetIO().WantCaptureMouse)
    {
	int hitIndex = RaycastScene(scene, resourceManager, rayOrigin, rayDirection);

	if (hitIndex != -1)
	{
	    GameObject* objectToRemove = scene->gameObjects[hitIndex];
	    if (hitIndex == selectedIndex)
	    {
		selectedIndex - 1;
	    } else if (hitIndex < selectedIndex)
	    {
		selectedIndex--;
	    }
	    scene->RemoveGameObject(objectToRemove);
	    Terminal::Log(LOG_INFO, "Object Deleted");
	}
    }
}

void EditorTools::Placement(Scene* scene, int gridX, int gridZ, ResourceManager* resourceManager)
{
    if (ImGui::IsMouseClicked(0) && !ImGui::GetIO().WantCaptureMouse)
    {
	std::string name = "Object_" + std::to_string(scene->gameObjects.size());
	GameObject* newObject = scene->CreateGameObject(name);

	newObject->position = glm::vec3((float)gridX + 0.5f, 0.0f, (float)gridZ + 0.5f);
	newObject->scale = glm::vec3(1.0f);
	newObject->rotation = glm::vec3(0.0f);

	MeshRenderer* meshRenderer = newObject->AddComponent<MeshRenderer>();
	// Default Cube setup
	std::filesystem::path cubePath = Paths::Models / "cube.obj";
	std::filesystem::path texturePath = Paths::Textures / "bricks.jpg";
	uint32_t cubeID = resourceManager->CreateMesh("cube", cubePath.string());
	meshRenderer->SetMesh(cubeID);

	uint32_t defaultTextureID = resourceManager->CreateTexture("default", texturePath.string());

	uint32_t defaultMaterialID =
	    resourceManager->CreateMaterial("material_bricks", defaultTextureID);
	meshRenderer->SetMaterial(defaultMaterialID);
	meshRenderer->isVisible = true;

	Terminal::Log(LOG_SUCCESS, "Empty Object Placed at: " + std::to_string(gridX) + ", " +
				       std::to_string(gridZ));
    }
}

void EditorTools::SelectObject(Scene* scene, int& selectedIndex, glm::vec3 rayOrigin,
			       glm::vec3 rayDirection, ResourceManager* resourceManager)
{
    if (ImGui::IsMouseClicked(0) && !ImGui::GetIO().WantCaptureMouse)
    {
	int hit = RaycastScene(scene, resourceManager, rayOrigin, rayDirection);
	if (hit != -1)
	{

	    if (selectedIndex != hit)
	    {
		selectedIndex = hit;
		Terminal::Log(LOG_INFO, "Selected Object ID:" + std::to_string(hit));
	    }

	} else
	{
	    if (selectedIndex != -1)
	    {
		selectedIndex = -1;
	    }
	}
    }
}
