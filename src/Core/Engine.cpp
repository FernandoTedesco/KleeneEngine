#include "Engine.h"
#include "Window.h"
#include "Camera.h"
#include "Input.h"
#include "Scenes/Scene.h"
#include "Graphics/Shader.h"
#include "Graphics/Renderer.h"
#include "Scenes/SceneManager.h"
#include "Resources/ResourceManager.h"
#include <filesystem>
#include "Development/Terminal.h"
#include "Development/Editor.h"
Engine::Engine(){ 

    terminal = new Terminal();
    terminal->WriteArt();
    window = new Window();
    
    camera = new Camera();
    resourceManager = new ResourceManager();
    sceneManager = new SceneManager();
    activeScene = new Scene();
    editor = new Editor(window, activeScene, sceneManager);

    std::filesystem::path currentPath = ResourceManager::FolderFinder("assets");
    shader = new Shader((currentPath/"assets/shaders/core.vert").string(), (currentPath/"assets/shaders/core.frag").string());
    renderer = new Renderer();
    
    
    sceneManager->LoadScene((currentPath/"assets/scenes/testar.kleene"), *activeScene);
    isRunning = true;
}



void Engine::Update()
{
    camera->ProcessInput();
}

void Engine::Run(){

    while(isRunning){

        isRunning = window->ProcessEvents();
        editor->BeginFrame();
        editor->DrawEditorUI();
        
        this->Update();
        
        renderer->Render(activeScene, resourceManager, shader, camera, window);
        editor->EndFrame();

        
        Input::UpdateLastState();
        Input::ResetMouseDelta();
        window->SwapBuffers();
    }
    

    

}

void Engine::SetScene(Scene*newScene){
    
    if(activeScene != nullptr)
    {
        delete activeScene;
        activeScene = newScene;
    }
    else
    {
        activeScene = newScene;
    }
    
           
}

Engine::~Engine(){

    delete editor;
    delete camera;
    delete shader;
    delete resourceManager;
    delete activeScene;
    delete sceneManager;
    delete renderer;
    delete window;
    delete terminal;


}

