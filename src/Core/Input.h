#ifndef INPUT_H
#define INPUT_H
#include "SDL.h"

class Input{

    public:

    Input() = delete;

    //Keyboard
    static bool keys[SDL_NUM_SCANCODES];
    static bool IsKeyDown(SDL_Scancode key);

    //Mouse
    //static float GetMouseX();
    //static float GetMouseY();
    static float GetMouseDeltaX();
    static float GetMouseDeltaY();

    static void UpdateMouseDelta(float deltaX, float deltaY);
    static void ResetMouseDelta();

    private:
    static float mouseX;
    static float mouseY;

    static float mouseDeltaX;
    static float mouseDeltaY;
    


};
#endif