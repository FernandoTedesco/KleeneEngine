#include "Editor.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_opengl3_loader.h"
#include "imgui_impl_sdl2.h"
#include "Core/Window.h"
#include <iostream>
#include "Scenes/SceneManager.h"
#include "Core/Camera.h"
#include "EditorGrid.h"
#include <psapi.h>
#include <windows.h>

Editor::Editor(Window* window, Scene* scene, SceneManager* sceneManager, Camera* camera){


    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(window->GetWindow(), window->GetglContext());
    ImGui_ImplOpenGL3_Init("#version 330");
    memset(answerSaveBuffer,0,sizeof(answerSaveBuffer));
    memset(answerLoadBuffer,0,sizeof(answerLoadBuffer));
    this->window = window;
    this->scene = scene;
    this->sceneManager = sceneManager;
    this->camera = camera;
    editorGrid = new EditorGrid(50);


}

static float GetRAMUsage()
{
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if(GetProcessMemoryInfo(GetCurrentProcess(),(PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
    {
        return static_cast<float>(pmc.WorkingSetSize)/(1024.0f * 1024.0f);
    }
    return 0.0f;
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
    ImGui::SetNextWindowSize(ImVec2(static_cast<float>(window->GetWidth()),80.0f));
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
    ImGui::NewLine();
    float fps = ImGui::GetIO().Framerate;
    float frameTime = 1000.0f/fps;
    float ram = GetRAMUsage();

    ImGui::Text("Camera position: %.2f, %.2f, %.2f", camera->GetCameraPos().x,camera->GetCameraPos().y,camera->GetCameraPos().z);
    ImGui::SameLine(0,20);
    ImGui::Text("FPS: %5.1f", fps);
    ImGui::SameLine(0,20);
    ImGui::Text("Frame Time: %6.3fms", frameTime);
    ImGui::SameLine(0,20);
    ImGui::Text("RAM Usage: %6.2fMB", ram);
    glm::vec3 rayDirection = camera->GetRayDirection(ImGui::GetIO().MousePos.x,ImGui::GetIO().MousePos.y, (float)window->GetWidth(),(float)window->GetHeight());
    glm::vec3 rayOrigin = camera->GetCameraPos();
    if(rayDirection.y < 0.0f)
    {
        float distance = -rayOrigin.y/rayDirection.y;
        glm::vec3 hitPoint = rayOrigin + (rayDirection * distance);

        int gridX = static_cast<int>(std::floor(hitPoint.x));
        int gridZ = static_cast<int>(std::floor(hitPoint.z));
        ImGui::Text("Grid Target: [%d,%d]", gridX, gridZ);

    }
    else
    {
        ImGui::Text("Grid Target: Sky");
    }
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
    delete this->editorGrid;
}