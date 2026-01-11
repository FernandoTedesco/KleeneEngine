
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "Imgui_impl_sdl2.h"
#include "Utils/Telemetry.h"
#include "Utils/MemoryTracker.h"
#include "implot.h"
#include <iostream>
#include "Editor.h"
#include "Core/Input.h"
#include "Utils/Math.h"
#include "Scenes/SceneManager.h"
#include "Components/MeshRenderer.h"
#include "Graphics/Mesh.h"
#include "EditorTools.h"
#include "EditorGrid.h"
#include "Core/Paths.h"
#include "Components/Light.h"
#include "Graphics/Material.h"
#include "Components/Terrain.h"
#include "Gui/EditorUI.h"
#include "Development/EditorState.h"
#include "Terminal.h"
Editor::Editor(Window* window, Scene* scene, SceneManager* sceneManager, Camera* camera,
	       ResourceManager* resourceManager)
{
    this->window = window;
    this->scene = scene;
    this->sceneManager = sceneManager;
    this->camera = camera;
    this->resourceManager = resourceManager;

    this->editorGrid = new EditorGrid(50);
    this->state.currentMode = EditorMode::SELECTION;
    this->tools = new EditorTools();

    EditorIcons loadedIcons = this->LoadIcons();
    this->editorUI = new EditorUI(loadedIcons);

    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(window->GetWindow(), window->GetglContext());
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGui::GetStyle().Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

    this->selectedEntityIndex = -1;

    gpuTelemetry.Init();
    memoryTracker.Init();
}

Editor::~Editor()
{
    ImGui_ImplSDL2_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    delete this->editorUI;
    delete this->tools;
    delete this->editorGrid;
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
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Editor::DrawEditorUI()
{
    float dt = 1.0f / ImGui::GetIO().Framerate;
    gpuTelemetry.Update(dt);
    memoryTracker.Update(dt);

    DrawTileMapSelector();

    auto onSaveAction = [&](const char* filename) {
	std::string fullPath = filename;
	if (fullPath.find(".Kleene") == std::string::npos)
	{
	    fullPath += ".Kleene";
	}
	this->sceneManager->SaveScene(fullPath, *this->scene, this->resourceManager);
	Terminal::Log(LOG_SUCCESS, "Scene Saved: " + fullPath);
    };
    auto onLoadAction = [&](const char* filename) {
	std::string fullPath = filename;
	if (fullPath.find(".Kleene") == std::string::npos)
	{
	    fullPath += ".Kleene";
	}
	Terminal::Log(LOG_INFO, "Requesting load for: " + fullPath);
	this->sceneManager->LoadScene(fullPath, *this->scene, this->resourceManager);
	Terminal::Log(LOG_SUCCESS, "Scene Saved: " + fullPath);
    };

    // lambda stuff tbr
    editorUI->Render(scene, resourceManager, selectedEntityIndex, onSaveAction, onLoadAction,
		     state);

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
    CheckToolState(rayOrigin, rayDirection);
}

void Editor::RenderHighlight(Shader* highlightShader)
{

    if (state.showGrid && editorGrid)
    {
	glEnable(GL_DEPTH_TEST);
	editorGrid->EditorGridDraw(camera, (float)window->GetWidth(), (float)window->GetHeight());
    }

    if (state.currentMode == EditorMode::DELETION || state.currentMode == EditorMode::PLACEMENT)
	return;
    if (highlightShader)
    {
	highlightShader->Use();
	for (GameObject* object : scene->gameObjects)
	{
	    if (object->GetComponent<Light>())
	    {
		tools->gizmo->DrawLightIcon(camera, object->position, highlightShader, window);
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
	tools->gizmo->Draw(camera, object->position, highlightShader, window);
    }
}

EditorIcons Editor::LoadIcons()
{

    std::filesystem::path iconPath = ResourceManager::FolderFinder("assets") / "assets/icons";

    uint32_t selectionIconID =
	resourceManager->CreateTexture("Icon_Selection", iconPath / "selection.png");

    uint32_t deleteIconID = resourceManager->CreateTexture("Icon_Delete", iconPath / "delete.png");

    uint32_t placementIconID =
	resourceManager->CreateTexture("Icon_Placement", iconPath / "placement.png");

    uint32_t translateIconID =
	resourceManager->CreateTexture("Icon_Translate", iconPath / "translate.png");

    uint32_t scaleIconID = resourceManager->CreateTexture("Icon_Scale", iconPath / "scale.png");

    uint32_t rotationIconID =
	resourceManager->CreateTexture("Icon_Rotation", iconPath / "rotation.png");

    uint32_t paintIconID = resourceManager->CreateTexture("Icon_Paint", iconPath / "paint.png");
    EditorIcons icons;

    if (auto tex = resourceManager->GetTexture(selectionIconID))
    {
	icons.selectionIcon =
	    (void*)(intptr_t)resourceManager->GetTexture(selectionIconID)->GetID();
    }
    if (auto tex = resourceManager->GetTexture(translateIconID))
    {
	icons.translateIcon =
	    (void*)(intptr_t)resourceManager->GetTexture(translateIconID)->GetID();
    }
    if (auto tex = resourceManager->GetTexture(deleteIconID))
    {
	icons.deleteIcon = (void*)(intptr_t)resourceManager->GetTexture(deleteIconID)->GetID();
    }
    if (auto tex = resourceManager->GetTexture(placementIconID))
    {
	icons.placementIcon =
	    (void*)(intptr_t)resourceManager->GetTexture(placementIconID)->GetID();
    }
    if (auto tex = resourceManager->GetTexture(scaleIconID))
    {
	icons.scaleIcon = (void*)(intptr_t)resourceManager->GetTexture(scaleIconID)->GetID();
    }
    if (auto tex = resourceManager->GetTexture(rotationIconID))
    {
	icons.rotationIcon = (void*)(intptr_t)resourceManager->GetTexture(rotationIconID)->GetID();
    }
    if (auto tex = resourceManager->GetTexture(paintIconID))
    {
	icons.paintIcon = (void*)(intptr_t)resourceManager->GetTexture(paintIconID)->GetID();
    }
    return icons;
}

void Editor::CheckToolState(const glm::vec3& rayOrigin, const glm::vec3& rayDirection)
{
    if (state.currentMode == EditorMode::TRANSLATE)
    {
	tools->UpdateTranslation(scene, selectedEntityIndex, rayOrigin, rayDirection,
				 resourceManager);
    } else if (state.currentMode == EditorMode::SCALE)
    {
	tools->UpdateScale(scene, selectedEntityIndex, rayOrigin, rayDirection, resourceManager);
    } else if (state.currentMode == EditorMode::ROTATION)
    {
	tools->UpdateRotation(scene, selectedEntityIndex, rayOrigin, rayDirection, resourceManager);
    } else if (state.currentMode == EditorMode::DELETION)
    {
	tools->DeleteObject(scene, selectedEntityIndex, rayOrigin, rayDirection, resourceManager);
    } else if (state.currentMode == EditorMode::PLACEMENT)
    {
	if (std::abs(rayDirection.y) > 0.001f)
	{
	    float t = -rayOrigin.y / rayDirection.y;
	    if (t > 0.0f)
	    {
		glm::vec3 hitPoint = rayOrigin + rayDirection * t;
		int gridX = (int)std::floor(hitPoint.x);
		int gridZ = (int)std::floor(hitPoint.z);

		tools->Placement(scene, gridX, gridZ, resourceManager);
	    }
	}
    } else if (state.currentMode == EditorMode::SELECTION)
    {

	tools->SelectObject(scene, selectedEntityIndex, rayOrigin, rayDirection, resourceManager);
    } else if (state.currentMode == EditorMode::PAINT)
    {
	if (ImGui::IsMouseDown(0) && !ImGui::GetIO().WantCaptureMouse)
	{
	    tools->PaintTerrain(scene, selectedEntityIndex, rayOrigin, rayDirection,
				resourceManager);
	}
    }
}

void Editor::DrawTileMapSelector()
{

    if (state.currentMode != EditorMode::PAINT)
	return;

    ImGui::Begin("TileMap Selector");

    static int tileSize = 32;
    if (tileSize < 8)
	tileSize = 8;

    uint32_t activeAtlasID = 0;

    if (selectedEntityIndex >= 0 && selectedEntityIndex < scene->gameObjects.size())
    {
	if (auto terrain = scene->gameObjects[selectedEntityIndex]->GetComponent<Terrain>())
	{
	    if (terrain->atlasTextureName.empty())
		terrain->atlasTextureName = "atlas.png";
	    Editor::DrawAtlasSelector(terrain);

	    activeAtlasID = resourceManager->CreateTexture(
		terrain->atlasTextureName, Paths::Tilemaps / terrain->atlasTextureName);
	    Texture* texture = resourceManager->GetTexture(activeAtlasID);

	    if (texture)
	    {
		terrain->atlasCols = texture->GetWidth() / tileSize;
		terrain->atlasRows = texture->GetHeight() / tileSize;
		if (terrain->atlasCols < 1)
		    terrain->atlasCols = 1;
		if (terrain->atlasRows < 1)
		    terrain->atlasRows = 1;

		if (auto rend =
			scene->gameObjects[selectedEntityIndex]->GetComponent<MeshRenderer>())
		{
		    if (auto material = resourceManager->GetMaterial(rend->materialID))
		    {
			material->diffuseMap = texture;
		    }
		}
	    }
	}
    }
    ImGui::Separator();
    Texture* texture = resourceManager->GetTexture(activeAtlasID);
    if (texture)
    {
	float textureWidth = (float)texture->GetWidth();
	float textureHeight = (float)texture->GetHeight();
	int atlasCols = (int)(textureWidth / tileSize);
	int atlasRows = (int)(textureHeight / tileSize);

	if (atlasCols < 1)
	    atlasCols = 1;
	if (atlasRows < 1)
	    atlasRows = 1;
	ImTextureID imTexID = (ImTextureID)(intptr_t)texture->GetID();
	int totalTiles = atlasCols * atlasRows;
	float buttonSize = 32.0f;

	ImGuiStyle& style = ImGui::GetStyle();
	float stepU = (float)tileSize / textureWidth;
	float stepV = (float)tileSize / textureHeight;
	float windowVisibleX = ImGui::GetWindowPos().x + ImGui::GetContentRegionAvail().x;

	for (int i = 0; i < totalTiles; i++)
	{
	    ImGui::PushID(i);
	    int col = i % atlasCols;
	    int row = i / atlasCols;
	    int flippedRow = (atlasRows - 1) - row;
	    int correctedID = flippedRow * atlasCols + col;

	    float u0 = col * stepU;
	    float v0 = row * stepV;
	    float u1 = u0 + stepU;
	    float v1 = v0 + stepV;

	    if (tools->selectedTileID == correctedID)
	    {
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 0, 1));
	    } else
	    {
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	    }
	    if (ImGui::ImageButton("##tile", imTexID, ImVec2(buttonSize, buttonSize),
				   ImVec2(u0, v0), ImVec2(u1, v1)))
	    {
		tools->SetSelectedTile(correctedID);
	    }
	    ImGui::PopStyleColor();
	    float lastButtonX = ImGui::GetItemRectMax().x;
	    float nextButtonX = lastButtonX + style.ItemSpacing.x + buttonSize;

	    if ((i + 1) % 8 != 0)
	    {
		ImGui::SameLine();
	    }
	    ImGui::PopID();
	}
    }

    ImGui::End();
}

void Editor::DrawAtlasSelector(Terrain* terrain)
{
    if (ImGui::BeginCombo("Atlas File", terrain->atlasTextureName.c_str()))
    {
	if (std::filesystem::exists(Paths::Tilemaps))
	{
	    for (const auto& entry : std::filesystem::directory_iterator(Paths::Tilemaps))
	    {
		if (entry.path().extension() == "png")
		{
		    std::string filename = entry.path().filename().string();
		    bool isSelected = (terrain->atlasTextureName == filename);
		    if (ImGui::Selectable(filename.c_str(), isSelected))
		    {
			terrain->atlasTextureName = filename;
		    }
		    if (isSelected)
			ImGui::SetItemDefaultFocus();
		}
	    }
	}
	ImGui::EndCombo();
    }
}