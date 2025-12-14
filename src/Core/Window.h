#ifndef WINDOW_H
#define WINDOW_H
#include <SDL.h>

class Window{
    public:
    Window();
    ~Window();
    void SwapBuffers();
    bool processEvents();
    private:
    SDL_Window* window;
    SDL_GLContext glContext;
    


};

#endif