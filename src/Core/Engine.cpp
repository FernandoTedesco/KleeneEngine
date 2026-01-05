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
#include "Graphics/ParticleManager.h"

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

    skyboxShader = new Shader((currentPath / "assets/shaders/skybox.vert").string(),
			      (currentPath / "assets/shaders/skybox.frag").string());

    camera = new Camera();
    activeScene = new Scene();
    resourceManager = new ResourceManager();
    sceneManager = new SceneManager();

    editor = new Editor(window, activeScene, sceneManager, camera, resourceManager, shader);
    terminal->SetEditorContext(editor);
    renderer = new Renderer();

    if (activeScene->skybox == nullptr)
    {
	std::vector<std::string> faces = {
	    (currentPath / "assets/textures/skyboxright.bmp").string(),
	    (currentPath / "assets/textures/skyboxleft.bmp").string(),
	    (currentPath / "assets/textures/skyboxtop.bmp").string(),
	    (currentPath / "assets/textures/skyboxbottom.bmp").string(),
	    (currentPath / "assets/textures/skyboxfront.bmp").string(),
	    (currentPath / "assets/textures/skyboxback.bmp").string()};
	activeScene->skyboxPaths = faces;
	activeScene->skybox = new Skybox(faces);
	for (int i = 0; i < faces.size(); i++)
	{
	    Terminal::Log(LOG_INFO, "[DEBUG PATH] Face " + std::to_string(i) + ": " + faces[i]);
	}
	Terminal::Log(LOG_RENDER, "Default Skybox Created");
    }
    framebuffer = new FrameBuffer(window->GetWidth(), window->GetHeight());
    screenShader = new Shader((currentPath / "assets/shaders/screen.vert").string(),
			      (currentPath / "assets/shaders/screen.frag").string());

    blurShader = new Shader((currentPath / "assets/shaders/screen.vert").string(),
			    (currentPath / "assets/shaders/blur.frag").string());

    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorbuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
	glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
	glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, window->GetWidth(), window->GetHeight(), 0,
		     GL_RGB, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
			       pingpongColorbuffers[i], 0);
    }
    shadowMap = new ShadowMap(4096, 4096);
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
    sceneManager->LoadScene("default.kleene", *activeScene, resourceManager);
    particleShader = new Shader((currentPath / "assets/shaders/particle.vert").string(),
				(currentPath / "assets/shaders/particle.frag").string());

    unsigned int particleTexID;
    glGenTextures(1, &particleTexID);
    glBindTexture(GL_TEXTURE_2D, particleTexID);
    unsigned char whitePixel[] = {255, 255, 255, 255};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    particleManager = new ParticleManager(particleShader, particleTexID, 5000);

    std::string playerTextureName = "Mk1.png";
    std::filesystem::path playerTexturePath = currentPath / "assets/textures" / playerTextureName;
    uint32_t playerTextureID = resourceManager->CreateTexture(playerTextureName, playerTexturePath);
    uint32_t playerMaterialID = resourceManager->CreateMaterial("Mat_Player", playerTextureID);
    uint32_t quadMeshID = 0;
    if (quadMeshID == 0)
    {
	quadMeshID = resourceManager->CreateMesh("quad", currentPath / "assets/models/quad.obj");
    }
    sceneManager->AddObject(*activeScene, glm::vec3(0, 1, 0), quadMeshID, playerMaterialID);
    GameObject* playerObject = &activeScene->gameObjects.back();
    playerObject->name = "Player";
    playerObject->scale = glm::vec3(1.0f, 1.2f, 1.0f);
    playerController = new PlayerController(playerObject, camera);

    isRunning = true;
    Terminal::Log(LOG_SUCCESS, "Kleene Engine Ready");
}

void Engine::Update()
{
    camera->ProcessInput();

    glm::vec3 targetPos(0.0f);
    if (playerController)
    {

	if (camera->GetCameraMode() == Camera::GAMEPLAY)
	{
	    playerController->Update(0.01f);
	}

	targetPos = playerController->GetPosition();
    }
    camera->CameraUpdate(0.01f, targetPos);
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
	particleManager->Update(0.016f, 2, glm::vec3(0.0f, 1.0f, 0.0f));
	count = terminal->UpdateConsoleInput();
	while (count > 0)
	{
	    terminal->ProcessConsoleInput();
	    count--;
	}

	activeScene->lights.clear();
	for (GameObject& object : activeScene->gameObjects)
	{
	    if (object.isActive && object.hasLightComponent)
	    {
		Light renderLight;
		renderLight.type = object.light.type;
		renderLight.color = object.light.color;
		renderLight.intensity = object.light.intensity;
		renderLight.constant = object.light.constant;
		renderLight.linear = object.light.linear;
		renderLight.quadratic = object.light.quadratic;
		renderLight.cutOff = glm::cos(glm::radians(object.light.cutOff));
		renderLight.outerCutOff = glm::cos(glm::radians(object.light.outerCutOff));

		renderLight.position = object.position;
		float pitch = glm::radians(object.rotation.x);
		float yaw = glm::radians(object.rotation.y);
		glm::vec3 direction;

		direction.x = -sin(yaw) * cos(pitch);
		direction.y = sin(pitch);
		direction.z = cos(yaw) * cos(pitch);
		renderLight.direction = glm::normalize(direction);

		activeScene->lights.push_back(renderLight);
	    }
	}
	glm::vec3 lightPosition = glm::vec3(0.0f, 20.0f, 0.0f);
	glm::vec3 lightDirection = glm::vec3(0.1, -1.0f, 0.1f);
	if (!activeScene->lights.empty())
	{
	    lightDirection = activeScene->lights[0].direction;
	    glm::vec3 gizmoPos = activeScene->lights[0].position;
	    lightPosition = gizmoPos - (lightDirection * 40.0f);
	    lightPosition = activeScene->lights[0].position;
	}
	glm::mat4 lightSpaceMatrix = shadowMap->GetLightSpaceMatrix(lightPosition, lightDirection);

	shadowShader->Use();
	shadowShader->SetMat4("lightSpaceMatrix", lightSpaceMatrix);
	glEnable(GL_DEPTH_TEST);
	shadowMap->Bind();
	glCullFace(GL_FRONT);
	renderer->Render(activeScene, resourceManager, shadowShader, camera, window, nullptr, true);

	glCullFace(GL_BACK);
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
	if (activeScene->skybox != nullptr)
	{
	    glDepthFunc(GL_LEQUAL);
	    skyboxShader->Use();

	    glDisable(GL_CULL_FACE);
	    glm::mat4 view = camera->GetViewMatrix();
	    glm::mat4 projection =
		camera->GetProjectionMatrix((float)window->GetWidth(), (float)window->GetHeight());
	    activeScene->skybox->Draw(skyboxShader, view, projection);
	    glEnable(GL_CULL_FACE);
	    glDepthFunc(GL_LESS);
	}
	particleShader->Use();
	particleShader->SetMat4("view", camera->GetViewMatrix());
	particleShader->SetMat4(
	    "projection",
	    camera->GetProjectionMatrix((float)window->GetWidth(), (float)window->GetHeight()));
	glDisable(GL_CULL_FACE);
	particleManager->Draw();
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (editor != nullptr)
	{
	    editor->RenderHighlight();
	}
	framebuffer->Unbind();

	bool horizontal = true, first_iteration = true;
	unsigned int amount = 10; // blur intensity (need testingg)
	blurShader->Use();
	for (unsigned int i = 0; i < amount; i++)
	{
	    glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
	    blurShader->SetInt("horizontal", horizontal);
	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, first_iteration ? framebuffer->GetBrightTextureID()
							 : pingpongColorbuffers[!horizontal]);
	    glBindVertexArray(rectVAO);
	    glDrawArrays(GL_TRIANGLES, 0, 6);
	    horizontal = !horizontal;
	    if (first_iteration)
		first_iteration = false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	screenShader->Use();
	glBindVertexArray(rectVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, framebuffer->GetTextureID());
	screenShader->SetInt("screenTexture", 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
	screenShader->SetInt("bloomBlur", 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, framebuffer->GetDepthTextureID());
	screenShader->SetInt("depthTexture", 2);
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
    delete particleManager;
    delete particleShader;
    delete resourceManager;
    delete activeScene;
    delete sceneManager;
    delete renderer;
    delete window;
    delete terminal;
    delete shadowMap;
    delete shadowShader;
}
