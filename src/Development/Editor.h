
#include <vector>
#include <string>
#include <filesystem>
class Window;
class Scene;
class SceneManager;
class Camera;
class EditorGrid;
class ResourceManager;
class Editor{

    public:
    Editor(Window* window, Scene* scene, SceneManager* sceneManager, Camera* camera, ResourceManager* resourceManager);
    void BeginFrame();
    void DrawEditorUI();
    void EndFrame();
    void PlaceObject(int gridX, int gridZ);
    EditorGrid* GetGrid(){return editorGrid;}
    ~Editor();
    bool listLoaded;
    std::vector<std::string>availableMeshes;
    std::vector<std::string>availableTextures;
    int selectedMeshIndex = 0;
    int selectedTextureIndex = 0;

    private:
    Window* window;
    SceneManager* sceneManager;
    Scene* scene;
    Camera* camera;
    ResourceManager* resourceManager;
    EditorGrid* editorGrid;
    char answerLoadBuffer[64];
    char answerSaveBuffer[64];
    std::vector<std::string> ScanDirectory(const std::filesystem::path directoryPath);
};