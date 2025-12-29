#pragma once
#include <map>
#include "SDL.h"
class Input
{

public:
    Input() = delete;

    // Keyboard
    enum engineKeyCode {
	W_KEY,
	S_KEY,
	A_KEY,
	D_KEY,
	F1_KEY,
	F2_KEY,
	F3_KEY,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6

    };

    static bool IsKeyDown(engineKeyCode key);
    static bool IsKeyPressed(engineKeyCode key);
    static void UpdateLastState();
    static float GetMouseDeltaX();
    static float GetMouseDeltaY();
    static void SetRelativeMouse(bool state);
    static void UpdateMouseDelta(float deltaX, float deltaY);
    static float GetMouseScroll();
    static void UpdateMouseScroll(float y);
    static void ResetMouseDelta();

private:
    static Uint8 lastFrameKeys[SDL_NUM_SCANCODES];
    static const Uint8* keyboardStateIndex;
    static float mouseX;
    static float mouseY;
    static float mouseScroll;

    static float mouseDeltaX;
    static float mouseDeltaY;
};
