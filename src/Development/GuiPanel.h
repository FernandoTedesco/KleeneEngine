#pragma once
#include "Scenes/Scene.h"
#include "Scenes/GameObject.h"
#include "Resources/ResourceManager.h"
#include "Components/Light.h"
#include "Gizmo.h"
#include "imgui.h"
#include <vector>
#include <string>

class GuiPanels
{
public:
    static void DrawHierarchy(Scene* scene, int& selectedIndex);
    static void DrawInspector(Scene* scene, int& selectedIndex, ResourceManager* resourceManager,
			      std::vector<std::string>& meshList, int& selectedMeshIndex,
			      std::vector<std::string>& textureList, int& selectedTextureIndex);

    static void DrawStats(float dt, float fps, size_t objectCount);

private:
    static void DrawTransform(GameObject* object);
    static void DrawMeshRenderer(GameObject* object, ResourceManager* resourceManager,
				 std::vector<std::string>& meshList, int& meshIndex,
				 std::vector<std::string>& textureList, int& textureIndex);
    static void DrawLight(GameObject* object);
};