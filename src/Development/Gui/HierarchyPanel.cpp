#include "HierarchyPanel.h"

void HierarchyPanel::Draw(Scene* scene, int& selectedIndex)
{
    float windowHeight = ImGui::GetIO().DisplaySize.y;
    ImGui::SetNextWindowPos(ImVec2(0, 80.0));
    ImGui::SetNextWindowSize(ImVec2(250, windowHeight - 80.0f));

    ImGui::Begin("Hierarchy", nullptr,
		 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    ImGui::Text("Scene Hierarchy");
    ImGui::Separator();
    for (int i = 0; i < scene->gameObjects.size(); i++)
    {
	GameObject* object = scene->gameObjects[i];
	if (!object)
	    continue;
	std::string label = object->name.empty() ? "Object " + std::to_string(i) : object->name;
	label += "##" + std::to_string(i);

	bool isSelected = (selectedIndex == i);
	if (ImGui::Selectable(label.c_str(), isSelected))
	{
	    selectedIndex = i;
	}
    }
    ImGui::End();
}