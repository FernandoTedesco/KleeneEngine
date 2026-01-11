#include "EditorUI.h"

EditorUI::EditorUI(const EditorIcons& icons)
{
    this->toolBar = new ToolBar();
    this->toolBar->SetIcons(icons);

    this->hierarchyPanel = new HierarchyPanel();
    this->inspectorPanel = new InspectorPanel();
}

EditorUI::~EditorUI()
{
    delete toolBar;
    delete hierarchyPanel;
    delete inspectorPanel;
}

void EditorUI::Render(Scene* scene, ResourceManager* resourceManager, int& selectedIndex,
		      std::function<void(const char*)> onSave,
		      std::function<void(const char*)> onLoad, EditorState& state)
{
    toolBar->Draw(ImGui::GetMainViewport()->Size.x, onSave, onLoad, state);

    if (state.showHierarchy)
    {
	hierarchyPanel->Draw(scene, selectedIndex);
    }
    if (state.showInspector)
    {
	if (selectedIndex >= 0 && selectedIndex < scene->gameObjects.size())
	{
	    inspectorPanel->Draw(scene, selectedIndex, resourceManager);
	}
    }
}

void EditorUI::BeginDockSpace()
{
}

void EditorUI::EndDockSpace()
{
}