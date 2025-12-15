#include "Input.h"


bool Input::keys[SDL_NUM_SCANCODES];
float Input::mouseDeltaX = 0.0f;
float Input::mouseDeltaY = 0.0f;

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


float Input::GetMouseDeltaX(){
    return(mouseDeltaX);
}
float Input::GetMouseDeltaY(){
    return(mouseDeltaY);
}

void Input::ResetMouseDelta(){
    mouseDeltaX = 0;
    mouseDeltaY = 0;

}

void Input::UpdateMouseDelta(float deltaX, float deltaY){
    mouseDeltaX += deltaX;
    mouseDeltaY += deltaY;
}