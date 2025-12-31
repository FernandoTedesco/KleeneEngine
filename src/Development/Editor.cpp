
#include <glad/glad.h>
#include "Core/Window.h"
#include "Editor.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
#include "Utils/Telemetry.h"
#include <iostream>
#include "Scenes/SceneManager.h"
#include "Core/Camera.h"
#include "EditorGrid.h"
#include <glm/gtc/matrix_transform.hpp>
#include <Resources/ResourceManager.h>
#include "Core/Input.h"
#include <glm/glm.hpp>
#include "Graphics/Mesh.h"
#include "Graphics/Shader.h"
#include "Utils/Math.h"
#include "Gizmo.h"
#include "Graphics/Material.h"
#include "implot.h"
// need more refactoring

Editor::Editor(Window* window, Scene* scene, SceneManager* sceneManager, Camera* camera,
	       ResourceManager* resourceManager, Shader* hightlightShader)
{

    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(window->GetWindow(), window->GetglContext());
    ImGui_ImplOpenGL3_Init("#version 330");
    memset(answerSaveBuffer, 0, sizeof(answerSaveBuffer));
    memset(answerLoadBuffer, 0, sizeof(answerLoadBuffer));
    this->window = window;
    this->scene = scene;
    this->sceneManager = sceneManager;
    this->camera = camera;
    this->resourceManager = resourceManager;
    this->currentMode = EditorMode::PLACEMENT;
    this->highlightShader = hightlightShader;
    this->gizmo = new Gizmo();
    editorGrid = new EditorGrid(50);

    this->listLoaded = false;
    this->selectedMeshIndex = 0;
    this->selectedTextureIndex = 0;

    ImGuiIO& io = ImGui::GetIO();

    gpuTelemetry.Init();
}

void Editor::BeginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void Editor::DrawEditorUI()
{
    float deltaTime = 1.0f / ImGui::GetIO().Framerate;
    gpuTelemetry.Update(deltaTime);

    this->HandleInput();
    glm::vec3 rayDirection =
	camera->GetRayDirection(ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y,
				(float)window->GetWidth(), (float)window->GetHeight());
    glm::vec3 rayOrigin = camera->GetCameraPos();

    this->TranslationModeUpdate(rayOrigin, rayDirection);
    this->DeletionModeUpdate(rayOrigin, rayDirection);
    this->ResizeModeUpdate(rayOrigin, rayDirection);
    this->RotationModeUpdate(rayOrigin, rayDirection);
    this->DrawHierarchy(window);
    if (!listLoaded)
    {
	std::filesystem::path currentPath = ResourceManager::FolderFinder("assets");

	availableMeshes = ScanDirectory(currentPath / "assets/models");
	availableTextures = ScanDirectory(currentPath / "assets/textures");

	listLoaded = true;
    }
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(static_cast<float>(window->GetWidth()), 80.0f));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(166 / 255.0f, 166 / 255.0f, 166 / 255.0f, 0.8f));

    ImGui::Begin("EditorUI", nullptr,
		 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar |
		     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
		     ImGuiWindowFlags_NoCollapse);
    ImGui::SetNextItemWidth(150);
    ImGui::InputText("###Filenamesave", answerSaveBuffer, sizeof(answerSaveBuffer));
    ImGui::SameLine();
    if (ImGui::Button("Save Scene"))
    {
	if (answerSaveBuffer[0] != '\0')
	{
	    std::cout << "[INFO] Save button pressed, attempting to save scene..." << std::endl;
	    std::filesystem::path temp = answerSaveBuffer;
	    sceneManager->SaveScene(temp, *scene, resourceManager);

	} else
	{
	    std::cout << "[ERROR] Cannot use an empty buffer" << std::endl;
	}
    }
    ImGui::PopStyleColor(1);
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(166 / 255.0f, 166 / 255.0f, 166 / 255.0f, 0.8f));

    ImGui::SetNextItemWidth(150);
    ImGui::InputText("###Filenameload", answerLoadBuffer, sizeof(answerLoadBuffer));
    ImGui::SameLine();
    if (ImGui::Button("Load Scene"))
    {
	if (answerLoadBuffer[0] != '\0')
	{
	    std::cout << "[INFO] Load button pressed, attempting to load scene..." << std::endl;
	    std::filesystem::path temp = answerLoadBuffer;
	    sceneManager->LoadScene(temp, *scene, resourceManager);
	} else
	{
	    std::cout << "[ERROR] Cannot use an empty buffer" << std::endl;
	}
    }

    ImGui::PopStyleColor(1);
    ImGui::SameLine();
    if (ImGui::Button("GPU Stats"))
    {
	ShowTelemetry = !ShowTelemetry;
    }
    ImGui::NewLine();
    float fps = ImGui::GetIO().Framerate;
    float frameTime = 1000.0f / fps;
    float ram = SystemMetrics::GetRAMUsage();

    ImGui::Text("Camera position: %.2f, %.2f, %.2f", camera->GetCameraPos().x,
		camera->GetCameraPos().y, camera->GetCameraPos().z);
    ImGui::SameLine(0, 20);
    ImGui::Text(
	"Zoom level: %.1f",
	camera->GetCameraPos().y); // A bit useless as of right now but nice to have nonetheless
    ImGui::SameLine(0, 20);
    ImGui::Text("FPS: %5.1f", fps);
    ImGui::SameLine(0, 20);
    std::string modeText = "UNKNOWN";
    if (currentMode == EditorMode::SELECTION)
    {
	modeText = "SELECTION";
    } else if (currentMode == EditorMode::DELETION)
    {
	modeText = "DELETION";
    } else if (currentMode == EditorMode::PLACEMENT)
    {
	modeText = "PLACEMENT";
    } else if (currentMode == EditorMode::TRANSLATE)
    {
	modeText = "TRANSLATION";
    } else if (currentMode == EditorMode::RESIZE)
    {
	modeText = "RESIZE";
    } else if (currentMode == EditorMode::ROTATION)
    {
	modeText = "ROTATION";
    }

    ImGui::Text("Editor Mode: %s", modeText.c_str());
    ImGui::SameLine(0, 20);
    ImGui::Text("Frame Time: %6.3fms", frameTime);
    ImGui::SameLine(0, 20);
    ImGui::Text("RAM Usage: %6.2fMB", ram);

    if (ImGui::IsMouseClicked(0) && !ImGui::GetIO().WantCaptureMouse)
    {
	if (currentMode == EditorMode::SELECTION)
	{
	    SelectObject(rayOrigin, rayDirection);
	}
    }

    if (rayDirection.y < 0.0f)
    {
	float distance = -rayOrigin.y / rayDirection.y;
	glm::vec3 hitPoint = rayOrigin + (rayDirection * distance);

	int gridX = static_cast<int>(std::floor(hitPoint.x));
	int gridZ = static_cast<int>(std::floor(hitPoint.z));
	ImGui::Text("Grid Target: [%d,%d]", gridX, gridZ);
	if (ImGui::IsMouseClicked(0) && !ImGui::GetIO().WantCaptureMouse)
	{

	    if (currentMode == EditorMode::PLACEMENT)
	    {
		this->PlaceObject(gridX, gridZ);
	    }
	    if (currentMode == EditorMode::RESIZE)
	    {
	    }
	    if (currentMode == EditorMode::ROTATION)
	    {
	    }
	}

    } else
    {
	ImGui::Text("Grid Target: Sky");
    }
    ImGui::End();
    if (ShowTelemetry)
    {
	DrawTelemetryWindow();
    }
    this->DrawInspector();
}
void Editor::DrawTelemetryWindow()
{
    if (!ImGui::Begin("Hardware Telemetry", &ShowTelemetry))
    {
	ImGui::End();
	return;
    }
    GpuSnapshot gpu = gpuTelemetry.GetCurrentState();
    ImGui::TextColored(ImVec4(0, 1, 0, 1), "[NVIDIA GPU]");
    ImGui::Separator();
    ImGui::Columns(2, "stats_cols", false);
    ImGui::Text("Temperature:");
    ImGui::NextColumn();
    ImGui::Text("%.0f C", gpu.temperature);
    ImGui::NextColumn();
    ImGui::Text("Core Load::");
    ImGui::NextColumn();
    ImGui::Text("%.1f %%", gpu.loadCore);
    ImGui::NextColumn();
    ImGui::Text("VRAM:");
    ImGui::NextColumn();
    ImGui::Text("%.1f %%", gpu.loadMemory);
    ImGui::NextColumn();
    ImGui::Text("Fan:");
    ImGui::NextColumn();
    ImGui::Text("%.0f %%", gpu.fanSpeed);
    ImGui::NextColumn();
    ImGui::Text("Power:");
    ImGui::NextColumn();
    ImGui::Text("%.1f W", gpu.powerWatts);
    ImGui::NextColumn();
    ImGui::Columns(1);
    ImGui::Spacing();
    const char* options[] = {"Temperature (C)", "Core Load (%)", "VRAM Load (%)", "Fan Speed (%)",
			     "Power (W)"};
    ImGui::Combo("Metric", &SelectedGraphMode, options, IM_ARRAYSIZE(options));
    const float* dataToPlot = nullptr;
    float yLimit = 100.0f;
    switch (SelectedGraphMode)
    {
    case 0:
	dataToPlot = gpuTelemetry.GetTemperatureHistory();
	yLimit = 100.0f;
	break;
    case 1:
	dataToPlot = gpuTelemetry.GetCoreLoadHistory();
	yLimit = 100.0f;
	break;
    case 2:
	dataToPlot = gpuTelemetry.GetVramHistory();
	yLimit = 100.0f;
	break;
    case 3:
	dataToPlot = gpuTelemetry.GetFanHistory();
	yLimit = 100.0f;
	break;
    case 4:
	dataToPlot = gpuTelemetry.GetTemperatureHistory();
	yLimit = 400.0f;
	break;
    }
    if (ImPlot::BeginPlot("##History", ImVec2(-1, 150)))
    {
	ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoLabel, ImPlotAxisFlags_AutoFit);
	ImPlot::SetupAxesLimits(0, HISTORY_SIZE, 0, yLimit, ImPlotCond_Always);
	if (dataToPlot)
	{
	    ImPlot::PlotLine("Data", dataToPlot, HISTORY_SIZE, 1.0, 0.0, 0,
			     gpuTelemetry.GetHistoryIndex());
	    ImPlot::EndPlot();
	}
    }
    ImGui::End();
}
void Editor::PlaceObject(int gridX, int gridZ)
{
    if (!availableMeshes.empty())
    {
	std::string meshName = availableMeshes[selectedMeshIndex];
	std::string textureName = availableTextures[selectedTextureIndex];

	std::filesystem::path currentPath = ResourceManager::FolderFinder("assets");
	uint32_t meshID =
	    resourceManager->CreateMesh(meshName, currentPath / "assets/models" / meshName);
	uint32_t textureID = resourceManager->CreateTexture(
	    textureName, currentPath / "assets/textures" / textureName);
	std::string materialName = "Mat_" + textureName;
	uint32_t materialID = resourceManager->CreateMaterial(materialName, textureID);

	float x = gridX + 0.5f;
	float z = gridZ + 0.5f;
	glm::vec3 position(x, 0, z);
	sceneManager->AddObject(*scene, position, meshID, materialID);
	std::cout << "[SUCCESS] Added Object at " << x << "," << z << std::endl;
    } else
    {
	std::cout << "[ERROR] There are no available meshes" << std::endl;
    }
}

void Editor::HandleInput()
{
    if (Input::IsKeyPressed(Input::KEY_1))
    {
	currentMode = EditorMode::SELECTION;
	std::cout << "[INFO] Selection Mode Enabled" << std::endl;
    }
    if (Input::IsKeyPressed(Input::KEY_2))
    {
	currentMode = EditorMode::DELETION;
	std::cout << "[INFO] Deletion Enabled" << std::endl;
    }
    if (Input::IsKeyPressed(Input::KEY_3))
    {
	currentMode = EditorMode::PLACEMENT;
	std::cout << "[INFO] Placement Mode Enabled" << std::endl;
    }
    if (Input::IsKeyPressed(Input::KEY_4))
    {
	currentMode = EditorMode::TRANSLATE;
	std::cout << "[INFO] Translation Mode Enabled" << std::endl;
    }
    if (Input::IsKeyPressed(Input::KEY_5))
    {
	currentMode = EditorMode::RESIZE;
	std::cout << "[INFO] Resize Mode Enabled" << std::endl;
    }
    if (Input::IsKeyPressed(Input::KEY_6))
    {
	currentMode = EditorMode::ROTATION;
	std::cout << "[INFO] Rotation Mode Enabled" << std::endl;
    }
    if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_D))
    {
	DuplicateSelectedObject();
    }
    if (!ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_F))
    {
	FocusOnSelectedObject();
    }
}

void Editor::EndFrame()
{
    ImGui::Render();
    ImDrawData* drawData = ImGui::GetDrawData();
    ImGui_ImplOpenGL3_RenderDrawData(drawData);
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

void Editor::RenderHighlight()
{
    if (this->selectedEntityIndex == -1 || this->selectedEntityIndex >= scene->gameObjects.size())
    {
	return;
    }
    if (!this->highlightShader)
	return;
    GameObject& object = scene->gameObjects[this->selectedEntityIndex];
    Mesh* mesh = resourceManager->GetMesh(object.meshID);
    if (!mesh)
	return;
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(2.0f);
    glDisable(GL_DEPTH_TEST);
    this->highlightShader->Use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, object.position);
    model = glm::rotate(model, glm::radians(object.rotation.x), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(object.rotation.y), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(object.rotation.z), glm::vec3(0, 0, 1));
    model = glm::scale(model, object.scale);
    glm::mat4 view = this->camera->GetViewMatrix();
    glm::mat4 projection =
	this->camera->GetProjectionMatrix((float)window->GetWidth(), (float)window->GetHeight());
    this->highlightShader->SetMat4("view", view);
    this->highlightShader->SetMat4("projection", projection);
    this->highlightShader->SetMat4("model", model);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    this->highlightShader->SetVec3("material.color", glm::vec4(0.0f, 0.5f, 1.0f, 0.4f));
    mesh->Draw();
    glDisable(GL_BLEND);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(2.0f);
    glDisable(GL_DEPTH_TEST);

    mesh->Draw();
    glEnable(GL_DEPTH_TEST);
    glLineWidth(1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    gizmo->Draw(camera, object.position, highlightShader, this->window);
}

void Editor::DrawInspector()
{
    // Inspector
    float sidebarWidth = 200.0f;
    float windowWidth = (float)window->GetWidth();
    float windowHeight = (float)window->GetHeight();

    ImGui::SetNextWindowPos(ImVec2(windowWidth - sidebarWidth, 80.0f));
    ImGui::SetNextWindowSize(ImVec2(sidebarWidth, windowHeight - 80.0f));
    ImGui::Begin("Inspector", nullptr,
		 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar |
		     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
		     ImGuiWindowFlags_NoCollapse);
    ImGui::Text("Asset Browser");
    ImGui::Separator();
    if (!availableMeshes.empty())
    {
	if (selectedMeshIndex >= availableMeshes.size())
	{
	    selectedMeshIndex = 0;
	}

	ImGui::Text("Mesh");
	if (ImGui::BeginCombo("##mesh", availableMeshes[selectedMeshIndex].c_str()))
	{
	    for (int i = 0; i < availableMeshes.size(); i++)
	    {
		bool isSelected = (selectedMeshIndex == i);
		if (ImGui::Selectable(availableMeshes[i].c_str(), isSelected))
		{
		    selectedMeshIndex = i;
		}

		if (isSelected)
		{
		    ImGui::SetItemDefaultFocus();
		}
	    }
	    ImGui::EndCombo();
	}
    }
    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    if (!availableTextures.empty())
    {
	if (selectedTextureIndex >= availableTextures.size())
	{
	    selectedTextureIndex = 0;
	}

	ImGui::Text("Texture");
	if (ImGui::BeginCombo("##texture", availableTextures[selectedTextureIndex].c_str()))
	{
	    for (int i = 0; i < availableTextures.size(); i++)
	    {
		bool isSelected = (selectedTextureIndex == i);
		if (ImGui::Selectable(availableTextures[i].c_str(), isSelected))
		    selectedTextureIndex = i;
		if (isSelected)
		{
		    ImGui::SetItemDefaultFocus();
		}
	    }
	    ImGui::EndCombo();
	}
	ImGui::Separator();
	ImGui::Text("Properties Inspector");
	if (this->selectedEntityIndex != -1 &&
	    this->selectedEntityIndex < scene->gameObjects.size())
	{
	    // Entity info
	    GameObject& selectedObject = scene->gameObjects[this->selectedEntityIndex];
	    ImGui::Spacing();
	    ImGui::Text("ID: %d", this->selectedEntityIndex);
	    ImGui::Text("Transform");
	    ImGui::DragFloat3("Position", &selectedObject.position[0], 0.1f);
	    ImGui::DragFloat3("Scale", &selectedObject.scale[0], 0.05f);
	    ImGui::DragFloat3("Rotation", &selectedObject.rotation[0], 1.0f);
	    ImGui::Spacing();
	    ImGui::Separator();
	    ImGui::Text("Texture Settings");
	    if (ImGui::BeginCombo("Texture", "Change Texture..."))
	    {
		for (int i = 0; i < availableTextures.size(); i++)
		{
		    bool isSelected = (selectedTextureIndex == i);
		    if (ImGui::Selectable(availableTextures[i].c_str(), isSelected))
		    {
			SetObjectTexture(selectedEntityIndex, availableTextures[i]);
		    }
		    if (isSelected)
		    {
			ImGui::SetItemDefaultFocus();
		    }
		}
		ImGui::EndCombo();
	    }

	    Material* material = resourceManager->GetMaterial(selectedObject.materialID);
	    if (material)
	    {
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Text("Material Properties");
		ImGui::ColorEdit3("Color Tint", &material->colorTint[0]);
		ImGui::DragFloat2("Tiling", &material->tiling[0], 0.05f);
		ImGui::DragFloat2("UV Offset", &material->offset[0], 0.01f);

		ImGui::Text("CurrentMaterial ID: %d", selectedObject.materialID);
		ImGui::Spacing();
	    }
	    // Material info

	    if (ImGui::Button("Duplicate Object", ImVec2(-1, 0)))
	    {
		DuplicateSelectedObject();
	    }
	    ImGui::Spacing();
	    if (ImGui::Button("Delete Object", ImVec2(-1, 0)))
	    {
		sceneManager->DeleteObject(*scene, this->selectedEntityIndex);
		selectedEntityIndex = -1;
	    }
	}

	ImGui::End();
    }
}

void Editor::TranslationModeUpdate(glm::vec3 rayOrigin, glm::vec3 rayDirection)
{
    if (currentMode != EditorMode::TRANSLATE)
    {
	return;
    }

    if (ImGui::IsMouseClicked(0) && !ImGui::GetIO().WantCaptureMouse)
    {
	if (selectedEntityIndex != -1)
	{
	    GameObject& object = scene->gameObjects[selectedEntityIndex];
	    currentAxis = gizmo->CheckHover(rayOrigin, rayDirection, object.position, 1.0f);
	} else
	{
	    currentAxis = GizmoAxis::NONE;
	}

	if (currentAxis != GizmoAxis::NONE)
	{
	    isDragging = true;
	    GameObject& object = scene->gameObjects[selectedEntityIndex];
	    draggingStartPosition = object.position;
	    ImVec2 mousePosition = ImGui::GetMousePos();
	    draggingStartMousePosition = glm::vec2(mousePosition.x, mousePosition.y);

	} else
	{
	    SelectObject(rayOrigin, rayDirection);
	    if (selectedEntityIndex != -1)
	    {
		currentAxis = GizmoAxis::NONE;
	    }
	}
    }

    if (isDragging && ImGui::IsMouseDown(0))
    {
	if (selectedEntityIndex == -1 || selectedEntityIndex >= scene->gameObjects.size())
	{
	    isDragging = false;
	    return;
	}

	GameObject& object = scene->gameObjects[selectedEntityIndex];
	ImVec2 currentMouse = ImGui::GetMousePos();
	float deltaX = currentMouse.x - draggingStartMousePosition.x;
	float deltaY = currentMouse.y - draggingStartMousePosition.y;
	float sensitivity = 0.02f;

	glm::vec3 targetPosition = draggingStartPosition;
	if (currentAxis == GizmoAxis::X)
	{
	    targetPosition.x += deltaX * sensitivity;
	} else if (currentAxis == GizmoAxis::Y)
	{

	    targetPosition.y -= deltaY * sensitivity;
	} else if (currentAxis == GizmoAxis::Z)
	{
	    targetPosition.z += deltaY * sensitivity;
	}
	if (ImGui::GetIO().KeyCtrl)
	{
	    float gridSize = 1.0f;
	    float halfGrid = 0.5f;
	    if (currentAxis == GizmoAxis::X)
	    {
		targetPosition.x = std::floor(targetPosition.x / gridSize) * gridSize + halfGrid;
	    } else if (currentAxis == GizmoAxis::Y)
	    {

		targetPosition.y = std::floor(targetPosition.y / gridSize) * gridSize + halfGrid;
	    } else if (currentAxis == GizmoAxis::Z)
	    {
		targetPosition.z = std::floor(targetPosition.z / gridSize) * gridSize + halfGrid;
	    }
	}
	object.position = targetPosition;
    }
    if (ImGui::IsMouseReleased(0))
    {
	isDragging = false;
	currentAxis = GizmoAxis::NONE;
    }
}

void Editor::SelectObject(glm::vec3 rayOrigin, glm::vec3 rayDirection)
{

    float closestDistance = FLT_MAX;
    int hitIndex = -1; // Empty click
    for (int i = 0; i < scene->gameObjects.size(); i++)
    {
	glm::vec3 objectPosition = scene->gameObjects[i].position;
	glm::vec3 halfSize = scene->gameObjects[i].scale * 0.5f;
	glm::vec3 aabbMin = objectPosition - halfSize;
	glm::vec3 aabbMax = objectPosition + halfSize;
	float distance = 0.0f;
	if (Math::RayAABBIntersection(rayOrigin, rayDirection, aabbMin, aabbMax, distance))
	{
	    if (distance < closestDistance)
	    {
		closestDistance = distance;
		hitIndex = i;
	    }
	}
    }
    if (hitIndex != -1)
    {
	std::cout << "[INFO] Object " << hitIndex << " selected!" << std::endl;
	this->selectedEntityIndex = hitIndex;
    } else
    {
	this->selectedEntityIndex = -1;
	std::cout << "[INFO] Empty click" << std::endl;
    }
}

void Editor::ResizeModeUpdate(glm::vec3 rayOrigin, glm::vec3 rayDirection)
{
    if (currentMode != EditorMode::RESIZE)
    {
	return;
    }

    if (ImGui::IsMouseClicked(0) && !ImGui::GetIO().WantCaptureMouse)
    {
	if (selectedEntityIndex != -1)
	{
	    GameObject& object = scene->gameObjects[selectedEntityIndex];
	    currentAxis = gizmo->CheckHover(rayOrigin, rayDirection, object.position, 1.0f);
	} else
	{
	    currentAxis = GizmoAxis::NONE;
	}

	if (currentAxis != GizmoAxis::NONE)
	{
	    isDragging = true;
	    GameObject& object = scene->gameObjects[selectedEntityIndex];
	    draggingStartPosition = object.position;
	    ImVec2 mousePosition = ImGui::GetMousePos();
	    draggingStartMousePosition = glm::vec2(mousePosition.x, mousePosition.y);
	    draggingStartScale = object.scale;

	} else
	{
	    SelectObject(rayOrigin, rayDirection);
	    if (selectedEntityIndex != -1)
	    {
		currentAxis = GizmoAxis::NONE;
	    }
	}
    }

    if (isDragging && ImGui::IsMouseDown(0))
    {
	if (selectedEntityIndex == -1 || selectedEntityIndex >= scene->gameObjects.size())
	{
	    isDragging = false;
	    return;
	}

	GameObject& object = scene->gameObjects[selectedEntityIndex];
	ImVec2 currentMouse = ImGui::GetMousePos();
	float deltaX = currentMouse.x - draggingStartMousePosition.x;
	float deltaY = currentMouse.y - draggingStartMousePosition.y;
	float sensitivity = 0.02f;

	glm::vec3 targetScale = draggingStartScale;
	if (currentAxis == GizmoAxis::X)
	{
	    targetScale.x += deltaX * sensitivity;
	} else if (currentAxis == GizmoAxis::Y)
	{

	    targetScale.y -= +deltaY * sensitivity;
	} else if (currentAxis == GizmoAxis::Z)
	{
	    targetScale.z += deltaY * sensitivity;
	}
	targetScale.x = std::max(0.1f, targetScale.x);
	targetScale.y = std::max(0.1f, targetScale.y);
	targetScale.z = std::max(0.1f, targetScale.z);
	if (ImGui::GetIO().KeyCtrl)
	{

	    float scaleStep = 0.5f;

	    if (currentAxis == GizmoAxis::X)
	    {
		targetScale.x = std::floor(targetScale.x / scaleStep) * scaleStep;
	    } else if (currentAxis == GizmoAxis::Y)
	    {

		targetScale.y = std::floor(targetScale.y / scaleStep) * scaleStep;
	    } else if (currentAxis == GizmoAxis::Z)
	    {
		targetScale.z = std::floor(targetScale.z / scaleStep) * scaleStep;
	    }
	}
	object.scale = targetScale;
    }
    if (ImGui::IsMouseReleased(0))
    {
	isDragging = false;
	currentAxis = GizmoAxis::NONE;
    }
}

void Editor::DeletionModeUpdate(glm::vec3 rayOrigin, glm::vec3 rayDirection)
{
    if (currentMode != EditorMode::DELETION)
	return;
    if (ImGui::IsMouseClicked(0) && !ImGui::GetIO().WantCaptureMouse)
    {
	float closestDistance = FLT_MAX;
	int hitIndex = -1;

	for (int i = 0; i < scene->gameObjects.size(); i++)
	{
	    glm::vec3 objectPosition = scene->gameObjects[i].position;
	    glm::vec3 halfSize = scene->gameObjects[i].scale * 0.5f;
	    glm::vec3 aabbMin = objectPosition - halfSize;
	    glm::vec3 aabbMax = objectPosition + halfSize;

	    float distance = 0.0f;
	    if (Math::RayAABBIntersection(rayOrigin, rayDirection, aabbMin, aabbMax, distance))
	    {
		if (distance < closestDistance)
		{
		    closestDistance = distance;
		    hitIndex = i;
		}
	    }
	}
	if (hitIndex != -1)
	{
	    std::cout << "[INFO] Deleted Object" << hitIndex << std::endl;
	    sceneManager->DeleteObject(*scene, hitIndex);
	    selectedEntityIndex = -1;
	}
    }
}

void Editor::RotationModeUpdate(glm::vec3 rayOrigin, glm::vec3 rayDirection)
{
    if (currentMode != EditorMode::ROTATION)
    {
	return;
    }

    if (ImGui::IsMouseClicked(0) && !ImGui::GetIO().WantCaptureMouse)
    {
	if (selectedEntityIndex != -1)
	{
	    GameObject& object = scene->gameObjects[selectedEntityIndex];
	    currentAxis = gizmo->CheckHover(rayOrigin, rayDirection, object.position, 1.0f);
	} else
	{
	    currentAxis = GizmoAxis::NONE;
	}

	if (currentAxis != GizmoAxis::NONE)
	{
	    isDragging = true;
	    GameObject& object = scene->gameObjects[selectedEntityIndex];
	    draggingStartPosition = object.position;
	    ImVec2 mousePosition = ImGui::GetMousePos();
	    draggingStartMousePosition = glm::vec2(mousePosition.x, mousePosition.y);
	    draggingStartRotation = object.rotation;

	} else
	{
	    SelectObject(rayOrigin, rayDirection);
	    if (selectedEntityIndex != -1)
	    {
		currentAxis = GizmoAxis::NONE;
	    }
	}
    }

    if (isDragging && ImGui::IsMouseDown(0))
    {
	if (selectedEntityIndex == -1 || selectedEntityIndex >= scene->gameObjects.size())
	{
	    isDragging = false;
	    return;
	}

	GameObject& object = scene->gameObjects[selectedEntityIndex];
	ImVec2 currentMouse = ImGui::GetMousePos();
	float deltaX = currentMouse.x - draggingStartMousePosition.x;
	float deltaY = currentMouse.y - draggingStartMousePosition.y;
	float sensitivity = 0.5f;

	glm::vec3 targetRotation = draggingStartRotation;

	// yes its a bit confusing but i found it more intuitive to be this way in the map editor,
	// will be changed in the future to be less messy to understand
	if (currentAxis == GizmoAxis::X)
	{
	    targetRotation.z += deltaX * sensitivity;

	} else if (currentAxis == GizmoAxis::Y)
	{
	    targetRotation.x += +deltaY * sensitivity;

	} else if (currentAxis == GizmoAxis::Z)
	{
	    targetRotation.y += deltaX * sensitivity;
	}

	if (ImGui::GetIO().KeyCtrl)
	{

	    float angleStep = 15.0f;
	    // confusion here too, but for now its overlooked
	    if (currentAxis == GizmoAxis::X)
	    {
		targetRotation.z = std::floor(targetRotation.z / angleStep) * angleStep;
	    } else if (currentAxis == GizmoAxis::Y)
	    {

		targetRotation.x = std::floor(targetRotation.x / angleStep) * angleStep;
	    } else if (currentAxis == GizmoAxis::Z)
	    {
		targetRotation.y = std::floor(targetRotation.y / angleStep) * angleStep;
	    }
	}
	object.rotation.x = targetRotation.x;
	object.rotation.y = targetRotation.y;
	object.rotation.z = targetRotation.z;
    }
    if (ImGui::IsMouseReleased(0))
    {
	isDragging = false;
	currentAxis = GizmoAxis::NONE;
    }
}

void Editor::DrawHierarchy(Window* window)
{
    float sidebarWidth = 200.0f;
    float windowHeight = (float)window->GetHeight();
    ImGui::SetNextWindowPos(ImVec2(0, 80.0));
    ImGui::SetNextWindowSize(ImVec2(sidebarWidth, windowHeight - 80.0f));
    ImGui::Begin("Hierarchy", nullptr,
		 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		     ImGuiWindowFlags_NoCollapse);
    ImGui::Text("Scene Hierarchy");
    ImGui::Separator();

    for (int i = 0; i < scene->gameObjects.size(); i++)
    {
	GameObject& object = scene->gameObjects[i];
	std::string label = object.name.empty() ? "Object " + std::to_string(i) : object.name;
	label += "##" + std::to_string(i);
	bool isSelected = (selectedEntityIndex == i);
	if (ImGui::Selectable(label.c_str(), isSelected))
	{
	    selectedEntityIndex = i;
	}
	if (isSelected)
	{
	    ImGui::SetItemDefaultFocus();
	}
    }
    ImGui::End();
}
void Editor::DuplicateSelectedObject()
{
    if (selectedEntityIndex == -1 || selectedEntityIndex >= scene->gameObjects.size())
    {
	return;
    }
    GameObject originalCopy = scene->gameObjects[selectedEntityIndex];

    sceneManager->AddObject(*scene, originalCopy.position, originalCopy.meshID,
			    originalCopy.materialID);
    GameObject& clone = scene->gameObjects.back();
    clone.rotation = originalCopy.rotation;
    clone.scale = originalCopy.scale;
    if (!originalCopy.name.empty())
    {
	clone.name = originalCopy.name + " (Clone)";
    } else
    {
	clone.name = "Object (Clone)";
	selectedEntityIndex = scene->gameObjects.size() - 1;
	std::cout << "[INFO] Object Duplicated successfully with ID: " << selectedEntityIndex
		  << std::endl;
    }
}

void Editor::SetObjectTexture(int objectIndex, std::string textureName)
{
    if (objectIndex == -1 || objectIndex >= scene->gameObjects.size())
	return;
    GameObject& object = scene->gameObjects[objectIndex];
    std::filesystem::path texturePath =
	ResourceManager::FolderFinder("assets") / "assets/textures" / textureName;
    uint32_t textureID = resourceManager->CreateTexture(textureName, texturePath);

    std::string materialName = "Mat_" + textureName;
    uint32_t materialID = resourceManager->CreateMaterial(materialName, textureID);

    object.materialID = materialID;
    std::cout << "[INFO] Changed Object " << objectIndex << " texture to:" << textureID
	      << std::endl;
}

void Editor::FocusOnSelectedObject()
{
    if (selectedEntityIndex == -1 || selectedEntityIndex >= scene->gameObjects.size())
    {
	return;
    }
    GameObject& object = scene->gameObjects[selectedEntityIndex];
    glm::vec3 focusOffset = glm::vec3(0.0f, 6.0f, 4.0f);
    glm::vec3 newPosition = object.position + focusOffset;
    camera->SetCameraPosition(newPosition);
    camera->SetCameraRotation(-60.0f, -90.0f);

    std::cout << "[INFO] Camera focused on object: " << selectedEntityIndex << std::endl;
}

Editor::~Editor()
{

    ImGui_ImplSDL2_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    delete this->editorGrid;
}