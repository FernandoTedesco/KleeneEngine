#include "ToolBar.h"
#include "Core/Window.h"
#include "Development/Terminal.h"

void ToolBar::Draw(float windowWidth, std::function<void(const char*)> onSaveRequest,
		   std::function<void(const char*)> onLoadRequest, EditorState& state)
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2((float)windowWidth, 80.0f));

    Stylize();
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
	ImGui::Spacing();
	ToolBar::CheckKeys(state);

	// IF BUTTON CLICK
	if (DrawModesButton(iconSelection, state.currentMode == EditorMode::SELECTION))
	{
	    state.currentMode = EditorMode::SELECTION;
	    Terminal::Log(LOG_INFO, "Mode Changed to: Selection");
	}
	ImGui::SameLine();
	if (DrawModesButton(iconDelete, state.currentMode == EditorMode::DELETION))
	{
	    state.currentMode = EditorMode::DELETION;
	    Terminal::Log(LOG_INFO, "Mode Changed to: Deletion");
	}
	ImGui::SameLine();
	if (DrawModesButton(iconPlacement, state.currentMode == EditorMode::PLACEMENT))
	{
	    state.currentMode = EditorMode::PLACEMENT;
	    Terminal::Log(LOG_INFO, "Mode Changed to: Placement");
	}
	ImGui::SameLine();
	if (DrawModesButton(iconTranslate, state.currentMode == EditorMode::TRANSLATE))
	{
	    state.currentMode = EditorMode::TRANSLATE;
	    Terminal::Log(LOG_INFO, "Mode Changed to: Translation");
	}
	ImGui::SameLine();
	if (DrawModesButton(iconRotation, state.currentMode == EditorMode::ROTATION))
	{
	    state.currentMode = EditorMode::ROTATION;
	    Terminal::Log(LOG_INFO, "Mode Changed to: Rotation");
	}
	ImGui::SameLine();
	if (DrawModesButton(iconScale, state.currentMode == EditorMode::SCALE))
	{
	    state.currentMode = EditorMode::SCALE;
	    Terminal::Log(LOG_INFO, "Mode Changed to: Scale");
	}
	ImGui::SameLine();
	if (DrawModesButton(iconPaint, state.currentMode == EditorMode::PAINT))
	{
	    state.currentMode = EditorMode::PAINT;
	    Terminal::Log(LOG_INFO, "Mode Changed to: Paint");
	}
    }

    ImGui::End();
    ImGui::PopStyleColor(6);
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
void ToolBar::SetIcons(const EditorIcons& icons)
{
    iconSelection = icons.selectionIcon;
    iconDelete = icons.deleteIcon;
    iconPlacement = icons.placementIcon;
    iconTranslate = icons.translateIcon;
    iconRotation = icons.rotationIcon;
    iconScale = icons.scaleIcon;
    iconPaint = icons.paintIcon;
}
bool ToolBar::DrawModesButton(void* iconID, bool isActive)
{
    if (!iconID)
	return false;

    ImGui::PushID(iconID);
    if (isActive)
    {
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
    } else
    {
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    }
    bool clicked = ImGui::ImageButton("##btn", (ImTextureID)iconID, ImVec2(32, 32), ImVec2(0, 1),
				      ImVec2(1, 0));

    ImGui::PopStyleColor();
    ImGui::PopID();
    return clicked;
}

void ToolBar::CheckKeys(EditorState& state)
{
    if (ImGui::IsKeyPressed(ImGuiKey_1))
    {
	state.currentMode = EditorMode::SELECTION;
    }
    if (ImGui::IsKeyPressed(ImGuiKey_2))
    {
	state.currentMode = EditorMode::DELETION;
    }
    if (ImGui::IsKeyPressed(ImGuiKey_3))
    {
	state.currentMode = EditorMode::PLACEMENT;
    }
    if (ImGui::IsKeyPressed(ImGuiKey_4))
    {
	state.currentMode = EditorMode::TRANSLATE;
    }
    if (ImGui::IsKeyPressed(ImGuiKey_5))
    {
	state.currentMode = EditorMode::ROTATION;
    }
    if (ImGui::IsKeyPressed(ImGuiKey_6))
    {
	state.currentMode = EditorMode::SCALE;
    }
    if (ImGui::IsKeyPressed(ImGuiKey_7))
    {
	state.currentMode = EditorMode::PAINT;
    }
}
void ToolBar::Stylize()
{
    ImVec4 fixedColor = ImVec4(0.05f, 0.2f, 0.05f, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_TitleBg, fixedColor);
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, fixedColor);

    ImGui::PushStyleColor(ImGuiCol_Header, fixedColor);
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, fixedColor);

    ImGui::PushStyleColor(ImGuiCol_Button, fixedColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, fixedColor);
}