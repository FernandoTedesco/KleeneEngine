#include "ToolBar.h"
#include "Core/Window.h"

void ToolBar::Draw(float windowWidth, std::function<void(const char*)> onSaveRequest,
		   std::function<void(const char*)> onLoadRequest, EditorState& state)
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2((float)windowWidth, 30.0f));

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
				   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
				   ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;

    ImGui::Begin("Toolbar", nullptr, windowFlags);

    if (ImGui::BeginMenuBar())
    {
	if (ImGui::BeginMenu("Scene"))
	{
	    if (ImGui::MenuItem("Save Scene..."))
		showSaveModal = true;
	    if (ImGui::MenuItem("Load Scene..."))
		showLoadModal = true;
	    ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("View"))
	{
	    ImGui::MenuItem("Hierarchy Panel", nullptr, &state.showHierarchy);
	    ImGui::MenuItem("Inspector Panel", nullptr, &state.showInspector);
	    ImGui::EndMenu();
	}
	ImGui::EndMenuBar();
    }

    ImGui::End();
    DrawSaveModal(onSaveRequest);
    DrawLoadModal(onLoadRequest);
}
void ToolBar::DrawSaveModal(std::function<void(const char*)> onConfirmSave)
{
    if (showSaveModal)
    {
	ImGui::OpenPopup("Save Scene");
	showSaveModal = false;
    }

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Save Scene", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
	ImGui::Text("Enter filename to save:");
	ImGui::InputText("##filename", saveBuffer, 64);
	ImGui::Separator();
	if (ImGui::Button("Save", ImVec2(120, 0)))
	{
	    if (onConfirmSave)
		onConfirmSave(saveBuffer);
	    showSaveModal = false;
	    ImGui::CloseCurrentPopup();
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel", ImVec2(120, 0)))
	{
	    showSaveModal = false;
	    ImGui::CloseCurrentPopup();
	}
	ImGui::EndPopup();
    }
}

void ToolBar::DrawLoadModal(std::function<void(const char*)> onConfirmLoad)
{
    if (showLoadModal)
    {
	ImGui::OpenPopup("Load Scene");
	showLoadModal = false;
    }

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Load Scene", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
	ImGui::Text("Enter filename to load:");
	ImGui::InputText("##filename", loadBuffer, 64);
	ImGui::Separator();
	if (ImGui::Button("Load", ImVec2(120, 0)))
	{
	    if (onConfirmLoad)
		onConfirmLoad(loadBuffer);
	    showLoadModal = false;
	    ImGui::CloseCurrentPopup();
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel", ImVec2(120, 0)))
	{
	    showLoadModal = false;
	    ImGui::CloseCurrentPopup();
	}
	ImGui::EndPopup();
    }
}