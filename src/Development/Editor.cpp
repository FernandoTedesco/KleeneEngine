#include "Editor.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_opengl3_loader.h"
#include "imgui_impl_sdl2.h"
#include "Core/Window.h"
#include <iostream>
Editor::Editor(Window* window){


    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(window->GetWindow(), window->GetglContext());
    ImGui_ImplOpenGL3_Init("#version 330");
    this->window = window;
}


void Editor::BeginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void Editor::DrawEditorUI()
{
    ImGui::SetNextWindowPos(ImVec2(0,0));
    ImGui::SetNextWindowSize(ImVec2(static_cast<float>(window->GetWidth()),40.0f));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(166/255.0f, 166/255.0f, 166/255.0f, 0.8f));
    
    ImGui::Begin("EditorUI",nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse);
    if(ImGui::Button("Save Scene"))
    {
        std::cout<<"[INFO] Save button pressed, Attempting to save scene..."<<std::endl;
    }
     ImGui::PopStyleColor(1);
     ImGui::SameLine();
     ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(166/255.0f, 166/255.0f, 166/255.0f, 0.8f));
    if(ImGui::Button("Load Scene"))
    {

    }
    ImGui::PopStyleColor(1);
    ImGui::End();
}

void Editor::EndFrame()
{
    ImGui::Render();
    ImDrawData* drawData = ImGui::GetDrawData();
    ImGui_ImplOpenGL3_RenderDrawData(drawData);
}

Editor::~Editor()
{
    
    ImGui_ImplSDL2_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
}