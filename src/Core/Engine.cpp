
#include "Engine.h"
#include <filesystem>
#include <SDL.h>
#include <iostream>

#include "Window.h"
#include "Camera.h"
#include "Input.h"

#include "Scenes/Scene.h"
#include "Scenes/SceneManager.h"

#include "Graphics/Shader.h"
#include "Graphics/Renderer.h"

#include "Resources/ResourceManager.h"

#include "Development/Terminal.h"
#include "Development/Editor.h"

#include "Graphics/ShadowMap.h"
#include "Graphics/ParticleManager.h"
#include "Graphics/PostProcessManager.h"

#include "Components/MeshRenderer.h"
#include "Components/Light.h"

Engine::Engine()
{

    // Core Systems
    terminal = new Terminal();
    terminal->WriteArt();
    terminal->ChangeLog();
    Terminal::Log(LOG_CORE, "Kleene Engine Initializing...");
    window = new Window();

    // Graphic Management
    renderer = new Renderer();

    unsigned int particleTexID;
    glGenTextures(1, &particleTexID);
    glBindTexture(GL_TEXTURE_2D, particleTexID);
    unsigned char whitePixel[] = {255, 255, 255, 255};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);
    particleManager = new ParticleManager(particleShader, particleTexID, 5000);

    // Scene Management
    camera = new Camera();
    activeScene = new Scene();
    sceneManager = new SceneManager();
    resourceManager = new ResourceManager();
    editor = new Editor(window, activeScene, sceneManager, camera, resourceManager, shader);
    terminal->SetEditorContext(editor);

    isRunning = true;
    Terminal::Log(LOG_SUCCESS, "Kleene Engine Ready");
}

void Engine::Update(float dt)
{
    camera->ProcessInput();
    camera->CameraUpdate(dt, glm::vec3(0.0f));
    if (activeScene != nullptr)
    {
	activeScene->Update(dt);
    }
    particleManager->Update(dt, 2, glm::vec3(0.0, 1.0f, 0.0f));
}

void Engine::Run()
{
    float lastFrameTime = 0.0f;
    while (isRunning)
    {
	float currentFrame = (float)SDL_GetTicks() / 1000.0f;
	float dt = currentFrame - lastFrameTime;
	lastFrameTime = currentFrame;

	if (dt > 0.1f)
	    dt = 0.1f;
	isRunning = window->ProcessEvents();
	editor->BeginFrame();
	editor->DrawEditorUI();
	// Terminal processing
	unsigned int count = terminal->UpdateConsoleInput();
	while (count > 0)
	{
	    terminal->ProcessConsoleInput();
	    count--;
	}
	this->Update(dt);
	renderer->RenderFrame(activeScene, camera, window, editor, resourceManager);
	editor->EndFrame();
	Input::UpdateLastState();
	Input::ResetMouseDelta();
	window->SwapBuffers();
    }
}

void Engine::SetScene(Scene* newScene)
{
    if (activeScene != nullptr)
    {
	delete activeScene;
    }
    activeScene = newScene;
}

Engine::~Engine()
{

    delete editor;
    delete terminal;
    delete postProcessManager;
    delete framebuffer;
    delete shadowMap;
    delete particleManager;
    delete renderer;
    delete sceneManager;
    delete activeScene;
    delete resourceManager;
    delete camera;
    delete window;
    delete shader;
    delete shadowShader;
    delete skyboxShader;
    delete particleShader;
}
