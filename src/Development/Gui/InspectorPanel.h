#pragma once
#include "Scenes/Scene.h"
#include "Resources/ResourceManager.h"
#include "imgui.h"
#include <vector>
#include <string>
#include <filesystem>

class InspectorPanel
{

public:
    InspectorPanel() = default;
    void Draw(Scene* scene, int& selectedEntityIndex, ResourceManager* resourceManager);

private:
    Texture* DrawTextureSelector(const char* label, Texture* currentTexture,
				 ResourceManager* resourceManager);
    std::vector<std::string> availableMeshes;
    std::vector<std::string> availableTextures;
    bool listLoaded = false;
    int selectedMeshIndex = 0;
    int selectedTextureIndex = 0;

    // could be unified sometime (?)
    void DrawTransform(GameObject* object);
    void DrawMeshRenderer(GameObject* object, ResourceManager* resourceManager);
    void DrawLight(GameObject* object);
    void DrawTerrain(GameObject* object);
    void DrawParticleSystem(GameObject* object);
    void DrawPlayerController(GameObject* object);
    void DrawSpriteRenderer(GameObject* object);
    void DrawSpriteAnimator(GameObject* object);
    void DrawCameraDirector(GameObject* object);

    void DrawAddComponentButton(GameObject* object, ResourceManager* resourceManager);
    void RefreshAssetLists();
    std::vector<std::string> ScanDirectory(const std::filesystem::path& directoryPath);
    void Stylize();
};
