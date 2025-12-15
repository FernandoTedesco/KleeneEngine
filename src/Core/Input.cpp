#include "Input.h"


bool Input::keys[SDL_NUM_SCANCODES];

bool Input::IsKeyDown(SDL_Scancode key){
    //Security check for valid key
    if(0<=key && key< SDL_NUM_SCANCODES)
    {
        return keys[key];
    }
    else
    {
        return false;
    }
}
