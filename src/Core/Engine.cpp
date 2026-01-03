#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "Engine.h"
#include "Window.h"
#include "Camera.h"
#include "Input.h"
#include "Scenes/Scene.h"
#include "Graphics/Shader.h"
#include "Graphics/Renderer.h"
#include "Scenes/SceneManager.h"
#include "Resources/ResourceManager.h"
#include <filesystem>
#include "Development/Terminal.h"
#include "Development/Editor.h"
#include "Graphics/ShadowMap.h"

Engine::Engine()
{

    terminal = new Terminal();
    terminal->WriteArt();
    terminal->ChangeLog();
    Terminal::Log(LOG_CORE, "Kleene Engine Initializing...");
    window = new Window();
    std::filesystem::path currentPath = ResourceManager::FolderFinder("assets");
    shader = new Shader((currentPath / "assets/shaders/core.vert").string(),
			(currentPath / "assets/shaders/core.frag").string());
    camera = new Camera();
    resourceManager = new ResourceManager();
    sceneManager = new SceneManager();
    activeScene = new Scene();
    editor = new Editor(window, activeScene, sceneManager, camera, resourceManager, shader);
    terminal->SetEditorContext(editor);
    renderer = new Renderer();
    sceneManager->AddLight(*activeScene, glm::vec3(-20.f, 30.0f, -20.0f), LightType::Directional);
    framebuffer = new FrameBuffer(window->GetWidth(), window->GetHeight());
    screenShader = new Shader((currentPath / "assets/shaders/screen.vert").string(),
			      (currentPath / "assets/shaders/screen.frag").string());

    shadowMap = new ShadowMap(2048, 2048);
    shadowShader = new Shader((currentPath / "assets/shaders/shadowDepth.vert").string(),
			      (currentPath / "assets/shaders/shadowDepth.frag").string());

    float quadVertices[] = {
	-1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f,

	-1.0f, 1.0f, 0.0f, 1.0f, 1.0f,	-1.0f, 1.0f, 0.0f, 1.0f, 1.0f,	1.0f, 1.0f};
    glGenVertexArrays(1, &rectVAO);
    glGenBuffers(1, &rectVBO);
    glBindVertexArray(rectVAO);
    glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    isRunning = true;
    Terminal::Log(LOG_SUCCESS, "Kleene Engine Ready");
}

void Engine::Update()
{
    camera->ProcessInput();
}

void Engine::Run()
{
    unsigned int count;
    while (isRunning)
    {

	isRunning = window->ProcessEvents();
	editor->BeginFrame();
	editor->DrawEditorUI();

	this->Update();
	count = terminal->UpdateConsoleInput();
	while (count > 0)
	{
	    terminal->ProcessConsoleInput();
	    count--;
	}

	glm::vec3 lightPosition = activeScene->lights[0].position;
	glm::vec3 lightTarget(0.0f, 0.0f, 0.0f);
	glm::vec3 lightDirection = glm::normalize(lightTarget - lightPosition);
	activeScene->lights[0].direction = lightDirection;
	glm::mat4 lightSpaceMatrix = shadowMap->GetLightSpaceMatrix(lightPosition, lightDirection);

	shadowShader->Use();
	shadowShader->SetMat4("lightSpaceMatrix", lightSpaceMatrix);
	glEnable(GL_DEPTH_TEST);
	shadowMap->Bind();

	renderer->Render(activeScene, resourceManager, shadowShader, camera, window, nullptr, true);

	shadowMap->Unbind(window->GetWidth(), window->GetHeight());

	framebuffer->Bind();
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader->Use();
	shader->SetMat4("lightSpaceMatrix", lightSpaceMatrix);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, shadowMap->GetMapID());
	shader->SetInt("shadowMap", 1);
	glActiveTexture(GL_TEXTURE0);
	if (editor->debugWireframeMode)
	{
	    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else
	{
	    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	renderer->Render(activeScene, resourceManager, shader, camera, window, editor->GetGrid(),
			 false);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (editor != nullptr)
	{
	    editor->RenderHighlight();
	}
	framebuffer->Unbind();

	glDisable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	screenShader->Use();
	glBindVertexArray(rectVAO);
	glBindTexture(GL_TEXTURE_2D, framebuffer->GetTextureID());
	glDrawArrays(GL_TRIANGLES, 0, 6);

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
	activeScene = newScene;
    } else
    {
	activeScene = newScene;
    }
}

Engine::~Engine()
{

    delete editor;
    delete camera;
    delete framebuffer;
    delete screenShader;
    delete shader;
    delete resourceManager;
    delete activeScene;
    delete sceneManager;
    delete renderer;
    delete window;
    delete terminal;
    delete shadowMap;
    delete shadowShader;
}
