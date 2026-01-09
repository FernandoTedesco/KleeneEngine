#include "EditorUI.h"

EditorUI::EditorUI(const EditorIcons& icons)
{
    this->toolBar = new ToolBar();
    this->toolBar->SetIcons(icons);

    this->hierarchyPanel = new HierarchyPanel();
    this->inspectorPanel = new InspectorPanel();
    this->editorState.showHierarchy = true;
    this->editorState.showInspector = true;
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
    toolBar->Draw(ImGui::GetMainViewport()->Size.x, onSave, onLoad, this->editorState);

    if (this->editorState.showHierarchy)
    {
	hierarchyPanel->Draw(scene, selectedIndex);
    }
    if (this->editorState.showInspector)
    {
	inspectorPanel->Draw(scene, selectedIndex, resourceManager);
    }
}

void EditorUI::BeginDockSpace()
{
}

void EditorUI::EndDockSpace()
{
}