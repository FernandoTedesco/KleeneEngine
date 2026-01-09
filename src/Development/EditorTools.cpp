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
#include "Resources/ResourceManager.h"
EditorTools::EditorTools()
    : isDragging(false), currentAxis(GizmoAxis::NONE), draggingStartMousePosition(0.0f),
      draggingStartPosition(0.0f), draggingStartScale(1.0f), draggingStartRotation(0.0f)
{
}

void EditorTools::UpdateTranslation(Scene* scene, int selectedIndex, glm::vec3 rayOrigin,
				    glm::vec3 rayDirection)
{
    if (selectedIndex == -1 || selectedIndex >= scene->gameObjects.size())
    {
	isDragging = false;
	return;
    }
    GameObject* object = scene->gameObjects[selectedIndex];
    if (ImGui::IsMouseClicked(0) && !ImGui::GetIO().WantCaptureMouse)
    {
	currentAxis = gizmo->CheckHover(rayOrigin, rayDirection, object->position, 1.0f);

	if (currentAxis != GizmoAxis::NONE)
	{
	    isDragging = true;
	    draggingStartPosition = object->position;
	    ImVec2 mPos = ImGui::GetMousePos();
	    draggingStartMousePosition = glm::vec2(mPos.x, mPos.y);
	}
    }
    if (isDragging && ImGui::IsMouseDown(0))
    {
	ImVec2 currentMouse = ImGui::GetMousePos();
	float deltaX = currentMouse.x - draggingStartMousePosition.x;
	float deltaY = currentMouse.y - draggingStartMousePosition.y;

	float sensitivity = 0.02f;

	glm::vec3 newPos = draggingStartPosition;
	if (currentAxis == GizmoAxis::X)
	    newPos.x += deltaX * sensitivity;
	if (currentAxis == GizmoAxis::Y)
	    newPos.x -= deltaY * sensitivity;
	if (currentAxis == GizmoAxis::Z)
	    newPos.x += deltaY * sensitivity;

	if (ImGui::GetIO().KeyCtrl)
	{
	    float gridSnap = 1.0f;
	    if (currentAxis == GizmoAxis::X)
		newPos.x = std::round(newPos.x / gridSnap);
	    if (currentAxis == GizmoAxis::Y)
		newPos.y = std::round(newPos.y / gridSnap);
	    if (currentAxis == GizmoAxis::Z)
		newPos.z = std::round(newPos.z / gridSnap);
	}
	object->position = newPos;
    }
    if (ImGui::IsMouseReleased(0))
    {
	isDragging = false;
	currentAxis = GizmoAxis::NONE;
    }
}

void EditorTools::UpdateRotation(Scene* scene, int selectedIndex, glm::vec3 rayOrigin,
				 glm::vec3 rayDirection)
{
    if (selectedIndex == -1 || selectedIndex >= scene->gameObjects.size())
    {
	isDragging = false;
	return;
    }
    GameObject* object = scene->gameObjects[selectedIndex];
    if (ImGui::IsMouseClicked(0) && !ImGui::GetIO().WantCaptureMouse)
    {
	currentAxis = gizmo->CheckHover(rayOrigin, rayDirection, object->position, 1.0f);

	if (currentAxis != GizmoAxis::NONE)
	{
	    isDragging = true;
	    draggingStartMousePosition = object->rotation;
	    ImVec2 mPos = ImGui::GetMousePos();
	    draggingStartMousePosition = glm::vec2(mPos.x, mPos.y);
	}
    }
    if (isDragging && ImGui::IsMouseDown(0))
    {
	ImVec2 currentMouse = ImGui::GetMousePos();
	float deltaX = currentMouse.x - draggingStartMousePosition.x;
	float deltaY = currentMouse.y - draggingStartMousePosition.y;

	float sensitivity = 0.5f;

	glm::vec3 newRot = draggingStartPosition;
	if (currentAxis == GizmoAxis::X)
	    newRot.z += deltaX * sensitivity;
	if (currentAxis == GizmoAxis::Y)
	    newRot.x -= deltaY * sensitivity;
	if (currentAxis == GizmoAxis::Z)
	    newRot.y += deltaX * sensitivity;

	if (ImGui::GetIO().KeyCtrl)
	{
	    float angleSnap = 15.0f;
	    newRot.x = std::round(newRot.x / angleSnap) * angleSnap;
	    newRot.y = std::round(newRot.y / angleSnap) * angleSnap;
	    newRot.z = std::round(newRot.z / angleSnap) * angleSnap;
	}
	object->rotation = newRot;
    }

    if (ImGui::IsMouseReleased(0))
    {
	isDragging = false;
	currentAxis = GizmoAxis::NONE;
    }
}

void EditorTools::UpdateScale(Scene* scene, int selectedIndex, glm::vec3 rayOrigin,
			      glm::vec3 rayDirection)
{
    if (selectedIndex == -1 || selectedIndex >= scene->gameObjects.size())
    {
	isDragging = false;
	return;
    }
    GameObject* object = scene->gameObjects[selectedIndex];
    if (ImGui::IsMouseClicked(0) && !ImGui::GetIO().WantCaptureMouse)
    {
	currentAxis = gizmo->CheckHover(rayOrigin, rayDirection, object->position, 1.0f);

	if (currentAxis != GizmoAxis::NONE)
	{
	    isDragging = true;
	    draggingStartScale = object->scale;
	    ImVec2 mPos = ImGui::GetMousePos();
	    draggingStartMousePosition = glm::vec2(mPos.x, mPos.y);
	}
    }
    if (isDragging && ImGui::IsMouseDown(0))
    {
	ImVec2 currentMouse = ImGui::GetMousePos();
	float deltaX = currentMouse.x - draggingStartMousePosition.x;
	float deltaY = currentMouse.y - draggingStartMousePosition.y;

	float sensitivity = 0.02f;

	glm::vec3 newScale = draggingStartScale;
	if (currentAxis == GizmoAxis::X)
	    newScale.x += deltaX * sensitivity;
	if (currentAxis == GizmoAxis::Y)
	    newScale.y -= deltaY * sensitivity;
	if (currentAxis == GizmoAxis::Z)
	    newScale.z += deltaY * sensitivity;
	newScale = glm::max(newScale, glm::vec3(0.1f));

	if (ImGui::GetIO().KeyCtrl)
	{
	    float scaleSnap = 0.5f;
	    newScale.x = std::round(newScale.x / scaleSnap) * scaleSnap;
	    newScale.y = std::round(newScale.y / scaleSnap) * scaleSnap;
	    newScale.z = std::round(newScale.z / scaleSnap) * scaleSnap;
	}
	object->scale = newScale;
    }

    if (ImGui::IsMouseReleased(0))
    {
	isDragging = false;
	currentAxis = GizmoAxis::NONE;
    }
}

void EditorTools::PaintTerrain(Scene* scene, int selectedIndex, glm::vec3 rayOrigin,
			       glm::vec3 rayDirection, ResourceManager* resourceManager,
			       int atlasRows, int atlasCols, int tileX, int tileY)
{

    if (selectedIndex < 0 || selectedIndex >= scene->gameObjects.size())
	return;
    GameObject* object = scene->gameObjects[selectedIndex];

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

	if (localHit.x >= mesh->boundsMin.x && localHit.x < mesh->boundsMax.x &&
	    localHit.z >= mesh->boundsMin.z && localHit.z < mesh->boundsMax.z)
	{
	    int mapTileX = (int)localHit.x;
	    int mapTileZ = (int)localHit.z;
	    int mapWidth = (int)mesh->boundsMax.x;

	    int vertexindex = mapTileZ * mapWidth + mapTileX;

	    int atlasCols = 16;
	    int atlasRows = 16;
	    float uStep = 1.0f / (float)atlasCols;
	    float vStep = 1.0f / (float)atlasRows;
	    float u0 = tileX * uStep;
	    float v0 = tileY * vStep;
	    float u1 = u0 + uStep;
	    float v1 = v0 + vStep;

	    glm::vec2 newUVs[4];
	    newUVs[0] = glm::vec2(u0, v0);
	    newUVs[1] = glm::vec2(u0, v1);
	    newUVs[2] = glm::vec2(u1, v1);
	    newUVs[3] = glm::vec2(u1, v0);

	    int vertexIndex = mapTileZ * mapWidth + mapTileX;
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
	glm::vec3 aabbMin, aabbMax;
	MeshRenderer* renderer = object->GetComponent<MeshRenderer>();
	if (renderer)
	{
	    Mesh* mesh = resourceManager->GetMesh(renderer->meshID);
	    if (mesh)
	    {
		aabbMin = object->position + (mesh->boundsMin * object->scale);
		aabbMax = object->position + (mesh->boundsMax * object->scale);

	    } else
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
	if (hitIndex != -1)
	{
	    Terminal::Log(LOG_INFO, "Selected Object ID: " + std::to_string(hitIndex));
	    return hitIndex;
	}
	return -1;
    }
    return -1;
}

void EditorTools::DeleteObject(Scene* scene, int& selectedIndex, glm::vec3 rayOrigin,
			       glm::vec3 rayDirection)
{
}