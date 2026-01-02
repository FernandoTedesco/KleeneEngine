#pragma once
#include <vector>
#include <string>
#include <filesystem>
#include "glm/glm.hpp"
#include "Gizmo.h"
#include "Utils/Telemetry.h"
#include "Utils/MemoryTracker.h"
class Window;
class Scene;
class SceneManager;
class Camera;
class EditorGrid;
class Shader;
class Gizmo;
class ResourceManager;

enum class EditorMode {
    SELECTION = 1,
    DELETION = 2,
    PLACEMENT = 3,
    TRANSLATE = 4,
    RESIZE = 5,
    ROTATION = 6
};

class Editor
{

public:
    Editor(Window* window, Scene* scene, SceneManager* sceneManager, Camera* camera,
	   ResourceManager* resourceManager, Shader* hightlightShader);
    bool debugWireframeMode = false;
    void BeginFrame();
    void DrawEditorUI();
    void EndFrame();
    void HandleInput();
    void RenderHighlight();
    void PlaceObject(int gridX, int gridZ);
    bool HasCollided(glm::vec3 rayOrigin, glm::vec3 rayDirection, glm::vec3 aabbMin,
		     glm::vec3 aabbMax, float& outDistance);
    EditorGrid* GetGrid()
    {
	return editorGrid;
    }
    ~Editor();
    bool listLoaded;
    std::vector<std::string> availableMeshes;
    std::vector<std::string> availableTextures;
    int selectedMeshIndex = 0;
    int selectedTextureIndex = 0;

private:
    void DuplicateSelectedObject();
    void FocusOnSelectedObject();

    GpuTelemetry gpuTelemetry;
    bool ShowTelemetry = false;
    void DrawTelemetryWindow();

    MemoryTracker memoryTracker;
    bool ShowMemoryTracker = false;
    void DrawMemoryWindow();

    Window* window;
    EditorMode currentMode;
    SceneManager* sceneManager;
    Scene* scene;
    Camera* camera;
    ResourceManager* resourceManager;
    EditorGrid* editorGrid;
    Gizmo* gizmo;
    int selectedEntityIndex = -1;
    int selectedLightIndex = -1;
    bool isDragging = false;
    glm::vec3 draggingStartPosition;
    glm::vec2 draggingStartMousePosition;
    glm::vec3 draggingStartScale;
    glm::vec3 draggingStartRotation;

    void SelectObject(glm::vec3 rayOrigin, glm::vec3 rayDirection);
    void DrawInspector();
    void DrawHierarchy(Window* window);
    void TranslationModeUpdate(glm::vec3 rayOrigin, glm::vec3 rayDirection);
    void DeletionModeUpdate(glm::vec3 rayOrigin, glm::vec3 rayDirection);
    void ResizeModeUpdate(glm::vec3 rayOrigin, glm::vec3 rayDirection);
    void RotationModeUpdate(glm::vec3 rayOrigin, glm::vec3 rayDirection);
    void SetObjectTexture(int objectIndex, std::string textureName);
    int SelectedGraphMode = 0;
    char answerLoadBuffer[64];
    char answerSaveBuffer[64];
    std::vector<std::string> ScanDirectory(const std::filesystem::path directoryPath);
    GizmoAxis currentAxis = GizmoAxis::NONE;
    Shader* highlightShader;
};