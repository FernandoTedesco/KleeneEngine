#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "Gizmo.h"

class Scene;
class Camera;
class Window;
class ResourceManager;
class EditorGrid;

class EditorTools
{
public:
    EditorTools();
    ~EditorTools();
    Gizmo* gizmo;
    void UpdateTranslation(Scene* scene, int& selectedIndex, glm::vec3 rayOrigin,
			   glm::vec3 rayDirection, ResourceManager* resourceManager);
    void UpdateRotation(Scene* scene, int& selectedIndex, glm::vec3 rayOrigin,
			glm::vec3 rayDirection, ResourceManager* ResourceManager);
    void UpdateScale(Scene* scene, int& selectedIndex, glm::vec3 rayOrigin, glm::vec3 rayDirection,
		     ResourceManager* resourceManager);
    void SelectObject(Scene* scene, int& selectedIndex, glm::vec3 rayOrigin, glm::vec3 rayDirection,
		      ResourceManager* resourceManager);
    void Placement(Scene* scene, int gridX, int gridZ, ResourceManager* ResourceManager);

    void DeleteObject(Scene* scene, int& selectedIndex, glm::vec3 rayOrigin, glm::vec3 rayDirection,
		      ResourceManager* resourceManager);

    void PaintTerrain(Scene* scene, int selectedIndex, glm::vec3 rayOrigin, glm::vec3 rayDirection,
		      ResourceManager* resourceManager);

    int selectedTileID = 0;
    void SetSelectedTile(int id)
    {
	selectedTileID = id;
    }
    int RaycastScene(Scene* scene, ResourceManager* resourceManager, glm::vec3 rayOrigin,
		     glm::vec3 rayDirection);

private:
    bool isDragging = false;
    GizmoAxis currentAxis = GizmoAxis::NONE;
    glm::vec3 draggingStartPosition;
    glm::vec2 draggingStartMousePosition;
    glm::vec3 draggingStartScale;
    glm::vec3 draggingStartRotation;
};
