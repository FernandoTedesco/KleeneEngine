#include "Editor.h"
#include "GuiPanel.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "Imgui_impl_sdl2.h"
#include "Utils/Telemetry.h"
#include "implot.h"
#include <iostream>
#include "Core/Input.h"
#include "Utils/Math.h"
#include "Scenes/SceneManager.h"
#include "Components/MeshRenderer.h"
#include "Graphics/Mesh.h"
#include "EditorTools.h"

Editor::Editor(Window* window, Scene* scene, SceneManager* sceneManager, Camera* camera,
	       ResourceManager* resourceManager, Shader* hightlightShader)
{
    this->window = window;
    this->scene = scene;
    this->sceneManager = sceneManager;
    this->camera = camera;
    this->resourceManager = resourceManager;
    this->highlightShader = highlightShader;

    this->gizmo = new Gizmo();
    this->editorGrid = new EditorGrid(50);
    this->currentMode = EditorMode::SELECTION;
    this->tools = new EditorTools();

    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(window->GetWindow(), window->GetglContext());
    ImGui_ImplOpenGL3_Init("#version 330");

    this->listLoaded = false;
    this->selectedEntityIndex = -1;
    this->selectedMeshIndex = 0;
    this->selectedTextureIndex = 0;

    gpuTelemetry.Init();
    memoryTracker.Init();
}

Editor::~Editor()
{
    ImGui_ImplSDL2_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    delete this->tools;
    delete this->editorGrid;
    delete this->gizmo;
}

void Editor::BeginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void Editor::EndFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDraw());
}

void Editor::DrawTopBar()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2((float)window->GetWidth(), 80.0f));
    ImGui::Begin("Toolbar, nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | "
		 "ImGuiWindowFlags_NoMove");

    static char saveBuffer[64] = "";
    ImGui::InputText("File", saveBuffer, 64);
    if (ImGui::Button("Save"))
    {
	sceneManager->SaveScene(saveBuffer, *scene, resourceManager);
    }
    ImGui::End();
}

void Editor::DrawEditorUI()
{
    float dt = 1.0f / ImGui::GetIO().Framerate;
    gpuTelemetry.Update(dt);
    memoryTracker.Update(dt);
    HandleInput();

    // Asset loading
    if (!listloaded)
    {
	std::filesystem::path currentPath = ResourceManager::FolderFinder("assets");
	availableMeshes = ScanDirectory(currentPath / "assets/models");
	abailableTextures = ScanDirectory(currentPath / "assets/textures");
	listLoaded = true;
    }

    float mouseX = ImGui::GetIO().MousePos.x;
    float mouseY = ImGui::GetIO().MousePos.y;
    glm::vec2 ndc = {(2.0f * mouseX) / window->GetWidth() - 1.0f,
		     1.0f - (2.0f * mouseY) / window->GetHeight()};
    glm::mat4 projection =
	camera->GetProjectionMatrix((float)window->GetWidth(), (float)window->GetHeight());
    glm::mat4 inverseVP = glm::inverse(projection * camera->GetViewMatrix());
    glm::vec4 worldPosition = inverseVP * glm::vec4(ndc.x, ndc.y, 1.0f, 1.0f);
    glm::vec3 rayDirection =
	glm::normalize(glm::vec3(worldPosition) / worldPosition.w - camera->GetCameraPos());
    glm::vec3 rayOrigin = camera->GetCameraPos();

    if (currentMode == EditorMode::TRANSLATE)
    {
	tools->UpdateTranslation(scene, selectedEntityIndex, rayOrigin, rayDirection);
    }
    if (currentMode == EditorMode::RESIZE)
    {
	tools->UpdateScale(scene, selectedEntityIndex, rayOrigin, rayDirection);
    } else if (currentMode == EditorMode::ROTATION)
    {
	tools->UpdateRotation(rayOrigin, rayDirection);
    } else if (currentMode == EditorMode::DELETION)
    {
	tools->DeleteObject()
    } else if (currentMode == EditorMode::SELECTION)
    {
	if (ImGui::IsMouseClicked(0) && !ImGui::GetIO().WantCaptureMouse)
	{
	    int hit = tools->RayCastScene(scene, resourceManager, rayOrigin, rayDirection);
	    if (hit != -1)
		selectedEntityIndex = hit;
	}
    }

    GuiPanels::DrawHierarchy(scene, selectedEntityIndex);
    GuiPanels::DrawInspector(scene, selectedEntityIndex, resourceManager, availableMeshes,
			     selectedMeshIndex, availableTextures, selectedTextureIndex);

    DrawTopBar();
}

void Editor::RenderHighLight()
{
    if (highlightShader)
    {
	highlightShader->Use();
	for (GameObject* object : scene->gameObjects)
	{
	    if (object->GetComponent<Light>())
	    {
		gizmo->DrawLightIcon(camera, object->GetPosition(), highlightShader, window);
	    }
	}
    }
    if (selectedEntityIndex >= 0 && selectedEntityIndex < scene->gameObjects.size())
    {
	GameObject* object = scene->gameObjects[selectedEntityIndex];
	if (!object)
	    return;
	MeshRenderer* meshRenderer = object->GetComponent<MeshRenderer>();
	if (meshRenderer && meshRenderer->isVisible)
	{
	    Mesh* mesh = resourceManager->GetMesh(meshRenderer->meshID);
	    if (mesh)
	    {
		glm::mat4 model = object->GetModelMatrix();
		highlightShader->SetMat4("model", model);
	    }
	}
	gizmo->Draw(camera, object->GetPosition(), hightLightShader, window);
    }
}

std::vector<std::string> Editor::ScanDirectory(const std::filesystem::path directoryPath)
{
    std::vector<std::string> files;

    if (!std::filesystem::exists(directoryPath))
    {
	return files;
    }
    for (const std::filesystem::directory_entry& entry :
	 std::filesystem::directory_iterator(directoryPath))
    {
	if (entry.is_regular_file())
	{
	    files.push_back(entry.path().filename().string());
	}
    }
    return files;
}