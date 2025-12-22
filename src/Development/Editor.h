
class Window;
class Scene;
class SceneManager;
class Camera;
class Editor{

    public:
    Editor(Window* window, Scene* scene, SceneManager* sceneManager, Camera* camera);
    void BeginFrame();
    void DrawEditorUI();
    void EndFrame();

    ~Editor();

    private:
    Window* window;
    SceneManager* sceneManager;
    Scene* scene;
    Camera* camera;
    char answerLoadBuffer[64];
    char answerSaveBuffer[64];
};