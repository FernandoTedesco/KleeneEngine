#include "Renderer.h"
#include <glad/glad.h>
#include <string>
#include <glm/gtc/matrix_transform.hpp>

#include "Core/Window.h"
#include "Core/Camera.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/Mesh.h"
#include "Graphics/Material.h"
#include "Development/EditorGrid.h"
#include "Resources/ResourceManager.h"
#include "Scenes/Scene.h"
#include "Scenes/GameObject.h"
#include "Components/MeshRenderer.h"
#include "Components/Light.h"
#include "Graphics/FrameBuffer.h"
#include "Development/Editor.h"
#include "PostProcessManager.h"
#include "Shadowmap.h"
Renderer::Renderer(Window* window)
{
    // Paths & Resources
    std::filesystem::path currentPath = ResourceManager::FolderFinder("assets");
    resourceManager = new ResourceManager();

    // Global Shaders
    mainShader = new Shader((currentPath / "assets/shaders/core.vert").string(),
			    (currentPath / "assets/shaders/core.frag").string());

    shadowShader = new Shader((currentPath / "assets/shaders/shadowDepth.vert").string(),
			      (currentPath / "assets/shaders/shadowDepth.frag").string());

    skyboxShader = new Shader((currentPath / "assets/shaders/skybox.vert").string(),
			      (currentPath / "assets/shaders/skybox.frag").string());

    particleShader = new Shader((currentPath / "assets/shaders/particle.vert").string(),
				(currentPath / "assets/shaders/particle.frag").string());

    shadowMap = new ShadowMap(4096, 4096);
    frameBuffer = new FrameBuffer(window->GetWidth(), window->GetHeight());
    postProcessManager = new PostProcessManager(window);
}
Renderer::~Renderer()
{
    delete postProcessManager;
    delete frameBuffer;
    delete shadowMap;
    delete mainShader;
    delete shadowShader;
    delete skyboxShader;
    delete particleShader;
}
void Renderer::RenderFrame(Scene* scene, ResourceManager* resourceManager, Shader* shader,
			   Camera* camera, Window* window, Editor* editor, bool isShadowPass)
{
    if (!scene)
	return;
    // Shadow
    PassShadowMap(scene, camera, window, resourceManager);
    // Color
    PassColor(scene, camera, window, editor, resourceManager);
    // Post Processing
    PassPostProcess();
}

void Renderer::PassShadowMap(Scene* scene, Camera* camera, Window* window,
			     ResourceManager* resourceManager)
{
    glm::vec3 lightPosition = glm::vec3(-2.0f, 20.0f, -1.0f);
    glm::vec3 lightDirection = glm::normalize(glm::vec3(0.1f, -1.0f, 0.1f));
    glm::mat4 lightSpaceMatrix = shadowMap->GetLightSpaceMatrix(lightPosition, lightDirection);

    shadowShader->Use();
    shadowShader->SetMat4("lightSpaceMatrix", lightSpaceMatrix);

    glViewport(0, 0, 4096, 4096);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMap->GetMapID());
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);

    for (GameObject* object : scene->gameObjects)
    {
	if (!object->isActive)
	    continue;
	MeshRenderer* meshRenderer = object->GetComponent<MeshRenderer>();
	if (meshRenderer && meshRenderer->meshID != 0 && meshRenderer->castShadows &&
	    meshRenderer->isVisible)
	{
	    DrawMesh(scene, meshRenderer, resourceManager, shadowShader, true);
	}
    }
    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, window->GetWidth(), window->GetHeight());
    mainShader->Use();
    mainShader->SetMat4("lightSpaceMatrix", lightSpaceMatrix);
}

void Renderer::PrepareScene(Camera* camera, Shader* shader, Window* window)
{
    int windowHeight = window->GetHeight();
    int windowWidth = window->GetWidth();
    glm::mat4 projectionMatrix =
	camera->GetProjectionMatrix((float)windowWidth, (float)windowHeight);
    glm::mat4 viewMatrix = camera->GetViewMatrix();
    shader->SetMat4("projection", projectionMatrix);
    shader->SetMat4("view", viewMatrix);
    shader->SetVec3("viewPos", camera->GetCameraPos());
}

void Renderer::SetupLights(Scene* scene, Shader* shader)
{
    unsigned int lightIndex = 0;
    const unsigned int MAX_LIGHTS = 16;
    for (GameObject* object : scene->gameObjects)
    {
	if (!object->isActive)
	    continue;

	Light* light = object->GetComponent<Light>();
	if (light)
	{
	    std::string number = std::to_string(lightIndex);
	    shader->SetInt("lights[" + number + "].type", (int)light->type);
	    shader->SetVec3("lights[" + number + "].position", light->GetPosition());
	    shader->SetVec3("lights[" + number + "].direction", light->GetDirection());
	    shader->SetVec3("lights[" + number + "].color", light->color);
	    shader->SetFloat("lights[" + number + "].intensity", light->intensity);

	    shader->SetFloat("lights[" + number + "].constant", light->constant);
	    shader->SetFloat("lights[" + number + "].linear", light->linear);
	    shader->SetFloat("lights[" + number + "].quadratic", light->quadratic);

	    shader->SetFloat("lights[" + number + "].cutOff",
			     glm::cos(glm::radians(light->cutOff)));
	    shader->SetFloat("lights[" + number + "].quadratic",
			     glm::cos(glm::radians(light->outerCutOff)));

	    lightIndex++;
	    if (lightIndex >= MAX_LIGHTS)
		break;
	}
    }
    shader->SetInt("numLights", lightIndex);
}
void Renderer::PassColor(Scene* scene, Camera* camera, Window* window, Editor* editor,
			 ResourceManager* resourceManager)
{
    frameBuffer->Bind();
    glEnable(GL_DEPTH_TEST);
    mainShader->Use();
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, shadowMap->GetMapID());
    mainShader->SetInt("shadowMap", 1);
    glActiveTexture(GL_TEXTURE0);

    if (editor && editor->debugWireframemode)
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    SetupLights(scene, mainShader);

    for (GameObject* object : scene->gameObjects)
    {
	if (!object->isActive)
	    continue;
	MeshRenderer* meshRenderer = object->GetComponent<MeshRenderer>();
	if (meshRenderer && meshRenderer->meshID != 0 && meshRenderer->isVisible)
	{
	    DrawMesh(scene, meshRenderer, resourceManager, mainShader, false);
	}
    }
    if (scene->skybox)
    {
	glDepthFunc(GL_LEQUAL);
	skyboxShader->Use();
	scene->skybox->Draw(
	    skyboxShader, camera->GetViewMatrix(),
	    camera->GetProjectionMatrix((float)window->GetWidth(), (float)window->GetHeight()));

	glDepthFunc(GL_LESS);
    }
    if (scene->particleManager)
    {
	particleShader->Use();
	particleShader->SetMat4("view", camera->GetViewMatrix());
	particleShader->SetMat4(
	    "projection",
	    camera->GetProjectionMatrix((float)window->GetWidth(), (float)window->GetHeight()));
	scene->particleManager->Draw();
    }
    if (editor)
	editor->RenderHighlight(mainShader);
    if (editor && editor->GetGrid())
	editor->GetGrid()->EditorGridDraw(camera, (float)window->GetWidth(),
					  (float)window->GetHeight());
    frameBuffer->Unbind();
}

void Renderer::PassPostProcess()
{
    unsigned int bloomTexture =
	postProcessManager->ExecuteBlur(frameBuffer->GetBrightTextureID(), 10);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    postProcessManager->RenderFinal(frameBuffer->GetTextureID(), bloomTexture);
}

void Renderer::DrawMesh(Scene* scene, MeshRenderer* meshRenderer, ResourceManager* resourceManager,
			Shader* shader, bool isShadowPass)
{
    Mesh* mesh = resourceManager->GetMesh(meshRenderer->meshID);
    Material* material = resourceManager->GetMaterial(meshRenderer->materialID);
    if (!mesh)
	return;

    if (!isShadowPass)
    {
	if (material)
	{
	    material->Use(shader);
	    shader->SetVec3("material.color", meshRenderer->colorTint);
	    shader->SetVec2("material.tiling", meshRenderer->textureTiling);
	    shader->SetVec2("material.offset", meshRenderer->textureOffset);
	    shader->SetFloat("material.specular", material->specular);
	    shader->SetFloat("material.shininess", material->shininess);
	} else
	{
	    shader->SetVec3("material.color", glm::vec3(1.0f, 0.0f, 1.0f));
	    shader->SetVec2("material.tiling", glm::vec2(1.0f, 1.0f));
	    shader->SetFloat("material.specular", 0.5f);
	    shader->SetFloat("material.shininess", 32.0f);
	}
    }
    if (meshRenderer->owner)
    {
	glm::mat4 modelMatrix = meshRenderer->owner->GetModelMatrix();
	shader->SetMat4("model", modelMatrix);
    }
    mesh->Draw();
}
