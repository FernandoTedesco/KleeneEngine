#include "Input.h"
#include "SDL.h"
#include "cstring"

float Input::mouseDeltaX = 0.0f;
float Input::mouseDeltaY = 0.0f;
float Input::mouseScroll = 0.0f;
Uint8 Input::lastFrameKeys[SDL_NUM_SCANCODES];
const Uint8* Input::keyboardStateIndex = SDL_GetKeyboardState(NULL);
static std::map<Input::engineKeyCode, SDL_Scancode> KeyCodeMap{
    {Input::W_KEY, SDL_SCANCODE_W},   {Input::S_KEY, SDL_SCANCODE_S},
    {Input::A_KEY, SDL_SCANCODE_A},   {Input::D_KEY, SDL_SCANCODE_D},
    {Input::F1_KEY, SDL_SCANCODE_F1}, {Input::F2_KEY, SDL_SCANCODE_F2},
    {Input::F3_KEY, SDL_SCANCODE_F3}, {Input::KEY_1, SDL_SCANCODE_1},
    {Input::KEY_2, SDL_SCANCODE_2},   {Input::KEY_3, SDL_SCANCODE_3},
    {Input::KEY_4, SDL_SCANCODE_4}

};
bool Input::IsKeyDown(engineKeyCode key)
{
    // Security check for valid key
    if (KeyCodeMap.count(key) != 0)
    {

	SDL_Scancode sdlKey = KeyCodeMap.at(key);
	if (keyboardStateIndex[sdlKey] == 1)
	{
	    return true;
	} else
	{
	    return false;
	}
    } else
    {
	return false;
    }
}

float Input::GetMouseDeltaX()
{
    return (mouseDeltaX);
}
float Input::GetMouseDeltaY()
{
    return (mouseDeltaY);
}
float Input::GetMouseScroll()
{
    return mouseScroll;
}
void Input::UpdateMouseScroll(float y)
{
    mouseScroll = y;
}
void Input::ResetMouseDelta()
{
    mouseDeltaX = 0;
    mouseDeltaY = 0;
    mouseScroll = 0;
}

void Input::UpdateMouseDelta(float deltaX, float deltaY)
{
    mouseDeltaX += deltaX;
    mouseDeltaY += deltaY;
}

bool Input::IsKeyPressed(engineKeyCode key)
{
    SDL_Scancode sdlKey = KeyCodeMap.at(key);
    if (lastFrameKeys[sdlKey] == 0 && keyboardStateIndex[sdlKey] == 1)
    {
	return true;
    } else
    {
	return false;
    }
}
void Input::UpdateLastState()
{
    memcpy(lastFrameKeys, keyboardStateIndex, SDL_NUM_SCANCODES);
}

void Input::SetRelativeMouse(bool state)
{
    if (state)
    {
	SDL_SetRelativeMouseMode(SDL_TRUE);
    } else
    {
	SDL_SetRelativeMouseMode(SDL_FALSE);
    }
}