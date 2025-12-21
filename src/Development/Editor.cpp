#include "Editor.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_opengl3_loader.h"
#include "imgui_impl_sdl2.h"
#include "Core/Window.h"
#include <iostream>
#include "Scenes/SceneManager.h"

Editor::Editor(Window* window, Scene* scene, SceneManager* sceneManager){


    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(window->GetWindow(), window->GetglContext());
    ImGui_ImplOpenGL3_Init("#version 330");
    memset(answerSaveBuffer,0,sizeof(answerSaveBuffer));
    memset(answerLoadBuffer,0,sizeof(answerLoadBuffer));
    this->window = window;
    this->scene = scene;
    this->sceneManager = sceneManager;
    

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
    ImGui::SetNextItemWidth(150);
    ImGui::InputText("###Filenamesave",answerSaveBuffer, sizeof(answerSaveBuffer));
    ImGui::SameLine();
    if(ImGui::Button("Save Scene"))
    {
        if(answerSaveBuffer[0] != '\0')
        {
            std::cout<<"[INFO] Save button pressed, attempting to save scene..."<<std::endl;
            std::filesystem::path temp = answerSaveBuffer;
            sceneManager->SaveScene(temp,*scene);

        }
        else
        {
            std::cout<<"[ERROR] Cannot use an empty buffer"<<std::endl;
        }
        
    }
     ImGui::PopStyleColor(1);
     ImGui::SameLine();
     ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(166/255.0f, 166/255.0f, 166/255.0f, 0.8f));

    ImGui::SetNextItemWidth(150);
    ImGui::InputText("###Filenameload",answerLoadBuffer, sizeof(answerLoadBuffer));
    ImGui::SameLine();
    if(ImGui::Button("Load Scene"))
    {
        if(answerLoadBuffer[0] != '\0')
        {
            std::cout<<"[INFO] Load button pressed, attempting to load scene..."<<std::endl;
            std::filesystem::path temp = answerLoadBuffer;
            sceneManager->LoadScene(temp,*scene);
        }
        else
        {
            std::cout<<"[ERROR] Cannot use an empty buffer"<<std::endl;
        }
        
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