#ifndef WINDOW_H
#define WINDOW_H
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

#endif