// FOR FUTURE DOCKING PURPOSES
#pragma once
#include "ToolBar.h"
#include "HierarchyPanel.h"
#include "InspectorPanel.h"
#include "imgui.h"
#include <functional>
class Scene;
class ResourceManager;

class EditorUI
{
public:
    EditorUI();
    ~EditorUI();
    void Render(Scene* scene, ResourceManager* resourceManager, int& selectedIndex,
		std::function<void(const char*)> onSave, std::function<void(const char*)> onLoad);

private:
    ToolBar* toolBar;
    HierarchyPanel* hierarchyPanel;
    InspectorPanel* inspectorPanel;

    void BeginDockSpace();
    void EndDockSpace();
};
