#include <SDL.h>
#include <glad/glad.h>
#include "Window.h"
#include <iostream>
#include <exception>
#include "Input.h"

Window::Window(){
    //SDL initialization
    if(SDL_Init(SDL_INIT_VIDEO)<0) 
    {
        std::cout<<"ERROR::WINDOW::SDL_INIT_VIDEO_FAILED"<<std::endl;
        std::cout<<"System Log:"<< SDL_GetError() <<std::endl;
        std::cout<<"Attempted to initialize SDL video event but failed"<<std::endl; 
        throw std::runtime_error("FATAL: SDL video has failed to initialize");
    }
     
    //SDL and OpenGL integration attributes   
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    //Window Builder
    window = SDL_CreateWindow("Kleene Engine",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,800,600,SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN);//| com suporte a opengl e seja visivel na tela
    if(!this->window) 
    {
        
        std::cout<<"ERROR::WINDOW::SDL_CREATE_WINDOW_FAILED"<<std::endl;
        std::cout<<"System Log:"<< SDL_GetError() <<std::endl;
        std::cout<<"Attempted to create a SDL window instance but failed"<<std::endl; 
        throw std::runtime_error("FATAL: SDL window has failed to be created");
    }

    //Fix mouse position
    SDL_SetRelativeMouseMode(SDL_TRUE);
    //SDL Context building and GLL Loader Check
    glContext = SDL_GL_CreateContext(this->window); 

    if(!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        std::cout<<"ERROR::WINDOW::GLAD_LOADER_FAILED"<<std::endl;
        std::cout<<"System Log:"<< SDL_GetError() <<std::endl;
        std::cout<<"Attempted load OpenGL function adresses but failed"<<std::endl; 
        throw std::runtime_error("FATAL: Failed to load OpenGL function adresses");

    }

}

void Window::SwapBuffers(){
 SDL_GL_SwapWindow(this->window);

}

bool Window::ProcessEvents(){
    bool isrunning = true;
    SDL_Event event;
    while(SDL_PollEvent(&event)) 
    {
     if(event.type == SDL_QUIT || event.type == SDL_KEYDOWN &&  SDL_SCANCODE_ESCAPE) isrunning = false;
     if(event.type == SDL_KEYDOWN)
     { 
        Input::keys[event.key.keysym.scancode] = true;
     }
     if(event.type == SDL_KEYUP)
     {     
        Input::keys[event.key.keysym.scancode] = false;
     }

    }

    return(isrunning);
}

int Window::GetWidth(){

    int width;
    SDL_GetWindowSize(window, &width,nullptr);
    return width;
}

int Window::GetHeight(){

    int height;
    SDL_GetWindowSize(window, nullptr, &height);
    return height;
}



Window::~Window()
{
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
    


   
   
