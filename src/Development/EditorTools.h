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

enum class EditorMode {
    SELECTION = 1,
    DELETION = 2,
    PLACEMENT = 3,
    TRANSLATE = 4,
    RESIZE = 5,
    ROTATION = 6,
    PAINT = 7
};

class EditorTools
{
public:
    EditorTools();
    ~EditorTools() = default;
    Gizmo* gizmo;
    void UpdateTranslation(Scene* scene, int selectedIndex, glm::vec3 rayOrigin,
			   glm::vec3 rayDirection);
    void UpdateRotation(Scene* scene, int selectedIndex, glm::vec3 rayOrigin,
			glm::vec3 rayDirection);
    void UpdateScale(Scene* scene, int selectedIndex, glm::vec3 rayOrigin, glm::vec3 rayDirection);

    void PlaceObject(Scene* scene, ResourceManager* resourceManager, int gridX, int gridZ,
		     const std::vector<std::string>& meshes, int meshIndex,
		     const std::vector<std::string>& textures, int textureIndex);
    void DeleteObject(Scene* scene, int& selectedIndex, glm::vec3 rayOrigin,
		      glm::vec3 rayDirection);

    void PaintTerrain(Scene* scene, int selectedIndex, glm::vec3 rayOrigin, glm::vec3 rayDirection,
		      ResourceManager* resourceManager, int atlasRows, int atlasCols, int tileX,
		      int tileY);

    void CreateProceduralTerrain(Scene* scene, ResourceManager* resourceManager);

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
