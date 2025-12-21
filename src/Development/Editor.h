
class Window;
class Scene;
class SceneManager;
class Editor{

    public:
    Editor(Window* window, Scene* scene, SceneManager* sceneManager);
    void BeginFrame();
    void DrawEditorUI();
    void EndFrame();

    ~Editor();

    private:
    Window* window;
    SceneManager* sceneManager;
    Scene* scene;
    char answerLoadBuffer[64];
    char answerSaveBuffer[64];
};