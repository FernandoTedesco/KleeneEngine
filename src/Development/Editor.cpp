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
#include <Resources/ResourceManager.h>
Editor::Editor(Window* window, Scene* scene, SceneManager* sceneManager, Camera* camera, ResourceManager* resourceManager){


    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(window->GetWindow(), window->GetglContext());
    ImGui_ImplOpenGL3_Init("#version 330");
    memset(answerSaveBuffer,0,sizeof(answerSaveBuffer));
    memset(answerLoadBuffer,0,sizeof(answerLoadBuffer));
    this->window = window;
    this->scene = scene;
    this->sceneManager = sceneManager;
    this->camera = camera;
    this->resourceManager = resourceManager;
    editorGrid = new EditorGrid(50);

    this->listLoaded = false;
    this->selectedMeshIndex = 0;
    this->selectedTextureIndex = 0;

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
    if(!listLoaded)
    {
        std::filesystem::path currentPath = ResourceManager::FolderFinder("assets");
  
        availableMeshes = ScanDirectory(currentPath/"assets/models");
        availableTextures = ScanDirectory(currentPath/"assets/textures");

        listLoaded = true;
    }
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
            sceneManager->SaveScene(temp,*scene, resourceManager);

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
            sceneManager->LoadScene(temp,*scene, resourceManager);
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
        if(ImGui::IsMouseClicked(0) && !ImGui::GetIO().WantCaptureMouse)
        {
            this->PlaceObject(gridX, gridZ);
        }

    }
    else
    {
        ImGui::Text("Grid Target: Sky");
    }
    ImGui::End();

    //Inspector
    float sidebarWidth = 100.0f;
    float windowWidth = (float)window->GetWidth();
    float windowHeight = (float)window->GetHeight();
    
    ImGui::SetNextWindowPos(ImVec2(windowWidth - sidebarWidth, 80.0f));
    ImGui::SetNextWindowSize(ImVec2(sidebarWidth, windowHeight - 80.0f));
    ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse);
    ImGui::Text("Asset Browser");
    ImGui::Separator();
    if(!availableMeshes.empty())
    {
        if(selectedMeshIndex >= availableMeshes.size()) 
        {
            selectedMeshIndex = 0;
        }

        ImGui::Text("Mesh");
        if(ImGui::BeginCombo("##mesh", availableMeshes[selectedMeshIndex].c_str()))
        {
            for(int i = 0; i<availableMeshes.size();i++)
            {
                bool isSelected = (selectedMeshIndex == i);
                if(ImGui::Selectable(availableMeshes[i].c_str(),isSelected))
                {
                    selectedMeshIndex = i;
                }
                
                if(isSelected) 
                {
                    ImGui::SetItemDefaultFocus();
                }

            }
            ImGui::EndCombo();
        }
        
    }
    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    if(!availableTextures.empty())
    {
        if(selectedTextureIndex >= availableTextures.size()) 
        {
            selectedTextureIndex = 0;
        }

        ImGui::Text("Texture");
        if(ImGui::BeginCombo("##texture", availableTextures[selectedTextureIndex].c_str()))
        {
            for(int i = 0; i<availableTextures.size();i++)
            {
                bool isSelected = (selectedTextureIndex == i);
                if(ImGui::Selectable(availableTextures[i].c_str(),isSelected))
                selectedTextureIndex = i;
                if(isSelected) 
                {
                    ImGui::SetItemDefaultFocus();
                }

            }
            ImGui::EndCombo();
        }
        
    }
    ImGui::End();




}

void Editor::PlaceObject(int gridX, int gridZ)
{
    if(!availableMeshes.empty())
    {
        std::string meshName = availableMeshes[selectedMeshIndex];
        std::string textureName = availableTextures[selectedTextureIndex];

        std::filesystem::path currentPath = ResourceManager::FolderFinder("assets");
        uint32_t meshID = resourceManager->CreateMesh(meshName,currentPath/"assets/models"/meshName);
        uint32_t textureID = resourceManager->CreateTexture(textureName,currentPath/"assets/textures"/textureName);

        float x = gridX + 0.5f;
        float z = gridZ + 0.5f;
        glm::vec3 position (x, 0, z);
        sceneManager->AddObject(*scene,position, meshID, textureID);
        std::cout<<"[SUCCESS] Added Object at"<< x <<","<< z<<std::endl;
    }
    else
    {
        std::cout<<"[ERROR] There are no available meshes"<<std::endl;
    }
    
    
    
}


void Editor::EndFrame()
{
    ImGui::Render();
    ImDrawData* drawData = ImGui::GetDrawData();
    ImGui_ImplOpenGL3_RenderDrawData(drawData);
}


std::vector<std::string> Editor::ScanDirectory(const std::filesystem::path directoryPath)
{
    std::vector<std::string> files;


    if(!std::filesystem::exists(directoryPath))
    {
        return files;
    }
    for(const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(directoryPath))
    {
        if(entry.is_regular_file())
        {
            files.push_back(entry.path().filename().string());
        }
    }
    return files;

}
Editor::~Editor()
{
    
    ImGui_ImplSDL2_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
    delete this->editorGrid;
}