#pragma once
class Camera;
class Window;
class ResourceManager;
class Scene;
class Renderer;
class Shader;
class Terminal;
class Engine{
    public:
    Engine();
    ~Engine();
    void SetScene(Scene*newScene);

    void Run();
    
    private:
    void HandleInput();
    void Update();

    Window* window;
    Camera* camera;
    Shader* shader;
    Renderer* renderer;
    ResourceManager* resourceManager;
    Scene* activeScene;
    Terminal* terminal;
    bool isRunning;

};