#pragma once
#include "Graphics/FrameBuffer.h"
#include "Graphics/Shadowmap.h"
#include "Graphics/Skybox.h"
#include "Graphics/ParticleManager.h"
class Camera;
class Window;
class ResourceManager;
class Scene;
class Renderer;
class Shader;
class Terminal;
class Editor;
class SceneManager;
class Engine
{

public:
    Engine();
    ~Engine();
    void SetScene(Scene* newScene);

    void Run();

private:
    void Update();
    FrameBuffer* framebuffer;
    Shader* screenShader;
    unsigned int rectVAO, rectVBO;
    Window* window;
    Camera* camera;
    Shader* shader;
    Renderer* renderer;
    ResourceManager* resourceManager;
    Scene* activeScene;
    Terminal* terminal;
    Editor* editor;
    ShadowMap* shadowMap;
    Shader* shadowShader;
    SceneManager* sceneManager;
    Shader* skyboxShader;
    Shader* blurShader;
    Shader* particleShader;
    ParticleManager* particleManager;
    unsigned int pingpongFBO[2];
    unsigned int pingpongColorbuffers[2];
    bool isRunning;
};