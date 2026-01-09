#include "HierarchyPanel.h"

void HierarchyPanel::Draw(Scene* scene, int& selectedIndex)
{
    float windowHeight = ImGui::GetIO().DisplaySize.y;
    float barHeight = 80.0f;
    ImGui::SetNextWindowPos(ImVec2(0, barHeight));
    ImGui::SetNextWindowSize(ImVec2(200, windowHeight - barHeight));

    this->Stylize();

    ImGui::Begin("Hierarchy", nullptr,
		 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    ImGui::Text("Scene Hierarchy");
    ImGui::Separator();
    for (int i = 0; i < scene->gameObjects.size(); i++)
    {
	GameObject* object = scene->gameObjects[i];
	if (!object)
	    continue;

	ImGui::PushID(object);
	std::string label = object->name.empty() ? "Object " + std::to_string(i) : object->name;

	bool isSelected = (selectedIndex == i);
	if (ImGui::Selectable(label.c_str(), isSelected))
	{
	    selectedIndex = i;
	}
	ImGui::PopID();
    }

    ImGui::End();
    ImGui::PopStyleColor(6);
}

void HierarchyPanel::Stylize()
{
    ImVec4 fixedColor = ImVec4(0.05f, 0.2f, 0.05f, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_TitleBg, fixedColor);
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, fixedColor);

    ImGui::PushStyleColor(ImGuiCol_Header, fixedColor);
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, fixedColor);

    ImGui::PushStyleColor(ImGuiCol_Button, fixedColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, fixedColor);
}