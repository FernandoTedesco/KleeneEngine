#include "EditorUI.h"

EditorUI::EditorUI()
{
    this->toolBar = new ToolBar();
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
		      std::function<void(const char*)> onLoad)
{
    toolBar->Draw(ImGui::GetMainViewport()->Size.x, onSave, onLoad);
    hierarchyPanel->Draw(scene, selectedIndex);
    inspectorPanel->Draw(scene, selectedIndex, resourceManager);
}

void EditorUI::BeginDockSpace()
{
}

void EditorUI::EndDockSpace()
{
}