// FOR FUTURE DOCKING PURPOSES
#pragma once
#include "ToolBar.h"
#include "HierarchyPanel.h"
#include "InspectorPanel.h"
#include "imgui.h"
#include <functional>
#include "Development/EditorState.h"
#include "Development/EditorResources.h"

class Scene;
class ResourceManager;

struct EditorLayoutState {
    bool showHierarchy = true;
    bool showInspector = true;
};

class EditorUI
{
public:
    EditorUI(const EditorIcons& icons);
    ~EditorUI();

public:
    void Render(Scene* scene, ResourceManager* resourceManager, int& selectedIndex,
		std::function<void(const char*)> onSave, std::function<void(const char*)> onLoad,
		EditorState& state);

private:
    ToolBar* toolBar;
    HierarchyPanel* hierarchyPanel;
    InspectorPanel* inspectorPanel;

    EditorLayoutState layoutState;

    void BeginDockSpace();
    void EndDockSpace();
};
