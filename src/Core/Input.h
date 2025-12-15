#ifndef INPUT_H
#define INPUT_H
#include "SDL.h"

class Input{
    public:
    Input() = delete;
    static bool keys[SDL_NUM_SCANCODES];
    static bool IsKeyDown(SDL_Scancode key);


};
#endif