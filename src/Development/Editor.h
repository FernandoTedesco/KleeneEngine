
#include <vector>
#include <string>
#include <filesystem>
#include "glm/glm.hpp"
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
    Window* window;
    EditorMode currentMode;
    SceneManager* sceneManager;
    Scene* scene;
    Camera* camera;
    ResourceManager* resourceManager;
    EditorGrid* editorGrid;
    Gizmo* gizmo;
    int selectedEntityIndex = -1;
    bool isDragging = false;
    glm::vec3 draggingStartPosition;
    glm::vec2 draggingStartMousePosition;
    void SelectObject(glm::vec3 rayOrigin, glm::vec3 rayDirection);
    void DrawInspector();
    void TranslationModeUpdate(glm::vec3 rayOrigin, glm::vec3 rayDirection);
    char answerLoadBuffer[64];
    char answerSaveBuffer[64];
    std::vector<std::string> ScanDirectory(const std::filesystem::path directoryPath);

    Shader* highlightShader;
};