#pragma once
#include <SDL.h>

class Window
{
public:
    Window();
    ~Window();
    int GetWidth();
    int GetHeight();
    void SwapBuffers();
    bool ProcessEvents();
    SDL_Window* GetWindow();
    SDL_GLContext GetglContext();

private:
    SDL_Window* window;
    SDL_GLContext glContext;
};
