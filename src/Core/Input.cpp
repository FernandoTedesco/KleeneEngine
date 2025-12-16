#include "Input.h"
#include "SDL.h"

float Input::mouseDeltaX = 0.0f;
float Input::mouseDeltaY = 0.0f;
static std::map<Input::engineKeyCode, SDL_Scancode> KeyCodeMap
{
    {Input::W_KEY, SDL_SCANCODE_W},
    {Input::S_KEY, SDL_SCANCODE_S},
    {Input::A_KEY, SDL_SCANCODE_A},
    {Input::D_KEY, SDL_SCANCODE_D}

};
bool Input::IsKeyDown(engineKeyCode key){
    //Security check for valid key
    if(KeyCodeMap.count(key) != 0)
    {

        const Uint8* keyboardStateIndex = SDL_GetKeyboardState(NULL);

        SDL_Scancode  sdlKey = KeyCodeMap.at(key);
        if(keyboardStateIndex[sdlKey] == 1){
            return true;
        }
        else{
            return false;
        }
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