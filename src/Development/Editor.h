
class Window;
class Scene;
class SceneManager;
class Camera;
class EditorGrid;
class Editor{

    public:
    Editor(Window* window, Scene* scene, SceneManager* sceneManager, Camera* camera);
    void BeginFrame();
    void DrawEditorUI();
    void EndFrame();
    EditorGrid* GetGrid(){return editorGrid;}
    ~Editor();

    private:
    Window* window;
    SceneManager* sceneManager;
    Scene* scene;
    Camera* camera;
    EditorGrid* editorGrid;
    char answerLoadBuffer[64];
    char answerSaveBuffer[64];
};