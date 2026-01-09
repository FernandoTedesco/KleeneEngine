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
    std::vector<std::string> availableMeshes;
    std::vector<std::string> availableTextures;
    bool listLoaded = false;
    int selectedMeshIndex = 0;
    int selectedTextureIndex = 0;

    void DrawTransform(GameObject* object);
    void DrawMeshRenderer(GameObject* object, ResourceManager* resourceManager);
    void DrawLight(GameObject* object);

    void RefreshAssetLists();
    std::vector<std::string> ScanDirectory(const std::filesystem::path& directoryPath);
    void Stylize();
};
