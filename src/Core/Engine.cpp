#include "Engine.h"
#include "Window.h"
#include "Camera.h"
#include "Input.h"
#include "Scenes/Scene.h"
#include "Graphics/Shader.h"
#include "Graphics/Renderer.h"
#include "Scenes/SceneLoader.h"
#include "Resources/ResourceManager.h"
#include <filesystem>
#include "Development/Terminal.h"
Engine::Engine(){ 

    terminal = new Terminal();
    terminal->WriteArt();
    window = new Window();
    camera = new Camera();
    resourceManager = new ResourceManager();
    std::filesystem::path currentPath = ResourceManager::FolderFinder("assets");
    shader = new Shader((currentPath/"assets/shaders/core.vert").string(), (currentPath/"assets/shaders/core.frag").string());
    renderer = new Renderer();
    activeScene = new Scene();
    SceneLoader sceneLoader;
    sceneLoader.LoadScene((currentPath/"assets/scenes/test.kleene"), *activeScene);
    isRunning = true;
}

void Engine::HandleInput(){
    
}

void Engine::Update()
{
    camera->ProcessInput();
}

void Engine::Run(){

    while(isRunning){

        isRunning = window->ProcessEvents();
        this->HandleInput();
        this->Update();
        
        renderer->Render(activeScene, resourceManager, shader, camera, window);
        
        
        
        Input::UpdateLastState();
        Input::ResetMouseDelta();
        
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

    delete terminal;
    delete camera;
    delete shader;
    delete resourceManager;
    delete renderer;
    delete window;


}

