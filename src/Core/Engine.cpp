#include "Engine.h"
#include "Window.h"
#include "Camera.h"
#include "Input.h"
#include "Scenes/Scene.h"
#include "Resources/ResourceManager.h"
Engine::Engine(){ 

    window = new Window();
    camera = new Camera();
    resourceManager = new ResourceManager();
    activeScene = nullptr;
}

void Engine::HandleInput(){
    if(Input::IsKeyPressed(Input::F1_KEY))camera->ChangeMode(Camera::FREECAM);
    if(Input::IsKeyPressed(Input::F2_KEY))camera->ChangeMode(Camera::SCENE_EDITOR);
}

void Engine::Update()
{
    camera->ProcessInput();
}

void Engine::Run(){

    while(isrunning){

        isrunning = window->ProcessEvents();
        this->HandleInput();
        this->Update();
        Input::UpdateLastState();
    }

}

void Engine::SetScene(){
    if(activeScene != nullptr)
    {
        delete activeScene;
    }
    else
    {
        activeScene = newScene;
        activeScene->Init();
    }
}

Engine::~Engine(){

    delete camera;
    delete window;


}

