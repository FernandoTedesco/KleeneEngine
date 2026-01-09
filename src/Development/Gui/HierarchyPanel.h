#pragma once
#include "Scenes/Scene.h"
#include "imgui.h"
#include <string>

class HierarchyPanel
{
public:
    void Draw(Scene* scene, int& selectedEntityIndex);

private:
    void Stylize();
};