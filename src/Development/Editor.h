#pragma once
#include <vector>
#include <string>
#include "EditorTools.h"
#include "GuiPanel.h"

class Window;
class Scene;
class SceneManager;
class Camera;
class EditorGrid;
class Shader;
class Gizmo;
class ResourceManager;
class Editor
{
public:
    Editor(Window* window, Scene* scene, SceneManager* sceneManager, Camera* camera,
	   ResourceManager* resourceManager, Shader* highllightShader);
    ~Editor();
    void BeginFrame();
    void EndFrame();
    void DrawEditorUI();
    EditorGrid* GetGrid()
    {
	return editorGrid;
    }
    void RenderHighlight();
    bool debugWireFrameMode = false;

private:
    void HandleInput();
    Window* window;
    Scene* scene;
    SceneManager* sceneManager;
    Camera* camera;
    ResourceManager* resourceManager;

    EditorGrid* editorGrid;
    Gizmo* gizmo;
    Shader* highlightShader;
    EditorTools* tools;
    EditorMode currentMode;
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

    std::vector<std::string> ScanDirectory(const std::string& directoryPath);
};