
class Camera;
class Window;
class ResourceManager;
class Scene;
class Engine{
    public:
    Engine();
    ~Engine();
   

    void Run();
    
    private:
    void HandleInput();
    void Update();
    void SetScene();
    Window* window;
    Camera* camera;
    ResourceManager* resourceManager;
    Scene* activeScene;
    
    bool isrunning;

};