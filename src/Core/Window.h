#pragma once
#include <SDL.h>

class Window{
    public:
    Window();
    ~Window();
    int GetWidth();
    int GetHeight();
    void SwapBuffers();
    bool ProcessEvents();
    
    private:
    SDL_Window* window;
    SDL_GLContext glContext;
    


};

