#pragma once
#include <vector>
#include <string>
#include "EditorTools.h"
#include <filesystem>
#include "Utils/MemoryTracker.h"
#include "Utils/Telemetry.h"
#include "EditorResources.h"
#include "Development/EditorState.h"
class Window;
class Scene;
class SceneManager;
class Camera;
class EditorGrid;
class Shader;
class Gizmo;
class ResourceManager;
class EditorUI;
class Terrain;

class Editor
{
public:
    Editor(Window* window, Scene* scene, SceneManager* sceneManager, Camera* camera,
	   ResourceManager* resourceManager);
    ~Editor();
    void BeginFrame();
    void EndFrame();
    void DrawEditorUI();
    void DrawTileMapSelector();
    EditorIcons LoadIcons();
    EditorGrid* GetGrid()
    {
	return editorGrid;
    }
    void RenderHighlight(Shader* highlightShader);
    bool debugWireframemode = false;

private:
    void CheckToolState(const glm::vec3& rayOrigin, const glm::vec3& rayDirection);
    void DrawAtlasSelector(Terrain* terrain);
    // Core Pointers
    Window* window;
    Scene* scene;
    SceneManager* sceneManager;
    Camera* camera;
    ResourceManager* resourceManager;

    // Editor Components
    EditorGrid* editorGrid;
    EditorTools* tools;
    EditorState state;

    // GUI modules
    EditorUI* editorUI;

    int selectedEntityIndex = -1;

    int atlasRows = 2;
    int atlasCols = 2;
    int selectedTileX = 0;
    int selectedTileY = 0;

    MemoryTracker memoryTracker;
    GpuTelemetry gpuTelemetry;
};