#pragma once
#include <vector>
#include <string>
#include "EditorTools.h"
#include "GuiPanel.h"
#include <filesystem>
#include "Utils/MemoryTracker.h"
#include "Utils/Telemetry.h"
class Window;
class Scene;
class SceneManager;
class Camera;
class EditorGrid;
class Shader;
class Gizmo;
class ResourceManager;
class ToolBar;

class Editor
{
public:
    Editor(Window* window, Scene* scene, SceneManager* sceneManager, Camera* camera,
	   ResourceManager* resourceManager);
    ~Editor();
    void BeginFrame();
    void EndFrame();
    void DrawEditorUI();

    EditorGrid* GetGrid()
    {
	return editorGrid;
    }
    void RenderHighlight(Shader* highlightShader);
    bool debugWireframemode = false;

private:
    void DrawTopBar();
    Window* window;
    Scene* scene;
    SceneManager* sceneManager;
    Camera* camera;
    ResourceManager* resourceManager;

    EditorGrid* editorGrid;
    Gizmo* gizmo;
    EditorTools* tools;
    EditorMode currentMode;

    // GUI
    ToolBar* toolBar;

    int selectedEntityIndex = -1;
    bool listLoaded = false;

    std::vector<std::string> availableMeshes;
    std::vector<std::string> availableTextures;

    int selectedMeshIndex = 0;
    int selectedTextureIndex = 0;

    int atlasRows = 2;
    int atlasCols = 2;
    int selectedTileX = 0;
    int selectedTileY = 0;

    char answerLoadBuffer[64];
    char answerSaveBuffer[64];

    MemoryTracker memoryTracker;
    GpuTelemetry gpuTelemetry;
    std::vector<std::string> ScanDirectory(const std::filesystem::path directoryPath);
};