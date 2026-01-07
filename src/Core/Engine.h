#pragma once
#include "Graphics/FrameBuffer.h"
#include "Graphics/ShadowMap.h"
#include "Graphics/Skybox.h"
#include "Graphics/ParticleManager.h"
#include "Graphics/PostProcessManager.h"
#include "Core/Window.h"
#include "Core/Camera.h"

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

    void Run();
    void SetScene(Scene* newScene);

private:
    void Update(float dt);
    void Render(float dt);

    // Subsystems
    Window* window;
    Terminal* terminal;
    Editor* editor;

    // Managers
    ResourceManager* resourceManager;
    SceneManager* sceneManager;
    PostProcessManager* postProcessManager;

    // Render Pipeline
    Renderer* renderer;
    Camera* camera;
    FrameBuffer* framebuffer;
    ShadowMap* shadowMap;
    ParticleManager* particleManager;

    // State
    Scene* activeScene;
    bool isRunning;

    // Global shaders
    Shader* shader;
    Shader* shadowShader;
    Shader* skyboxShader;
    Shader* particleShader;
};