#pragma once
#include <map>
class Input{

    public:

    Input() = delete;

    //Keyboard
    enum engineKeyCode{
        W_KEY,
        S_KEY,
        A_KEY,
        D_KEY

    };
    
    static bool IsKeyDown(engineKeyCode key);

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
