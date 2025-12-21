#include "Terminal.h"
#include <Windows.h>
#include <stdio.h>
#include <iostream>

static const char* signature = R"(
 █████   ████ █████       ██████████ ██████████ ██████   █████ ██████████    ██████████ ██████   █████   █████████  █████ ██████   █████ ██████████
░░███   ███░ ░░███       ░░███░░░░░█░░███░░░░░█░░██████ ░░███ ░░███░░░░░█   ░░███░░░░░█░░██████ ░░███   ███░░░░░███░░███ ░░██████ ░░███ ░░███░░░░░█
 ░███  ███    ░███        ░███  █ ░  ░███  █ ░  ░███░███ ░███  ░███  █ ░     ░███  █ ░  ░███░███ ░███  ███     ░░░  ░███  ░███░███ ░███  ░███  █ ░ 
 ░███████     ░███        ░██████    ░██████    ░███░░███░███  ░██████       ░██████    ░███░░███░███ ░███          ░███  ░███░░███░███  ░██████   
 ░███░░███    ░███        ░███░░█    ░███░░█    ░███ ░░██████  ░███░░█       ░███░░█    ░███ ░░██████ ░███    █████ ░███  ░███ ░░██████  ░███░░█   
 ░███ ░░███   ░███      █ ░███ ░   █ ░███ ░   █ ░███  ░░█████  ░███ ░   █    ░███ ░   █ ░███  ░░█████ ░░███  ░░███  ░███  ░███  ░░█████  ░███ ░   █
 █████ ░░████ ███████████ ██████████ ██████████ █████  ░░█████ ██████████    ██████████ █████  ░░█████ ░░█████████  █████ █████  ░░█████ ██████████
░░░░░   ░░░░ ░░░░░░░░░░░ ░░░░░░░░░░ ░░░░░░░░░░ ░░░░░    ░░░░░ ░░░░░░░░░░    ░░░░░░░░░░ ░░░░░    ░░░░░   ░░░░░░░░░  ░░░░░ ░░░░░    ░░░░░ ░░░░░░░░░░ 
                                                                                                                                                   
                                                                                                                                                   
)";




Terminal::Terminal()
{
    FreeConsole();
    if(!AllocConsole()){
        if(GetLastError() != 5)
        {
            return;
        }
    }
    //Terminal Creation
    outputHandle = (void*)GetStdHandle(STD_OUTPUT_HANDLE);
    windowHandle = (void*)GetConsoleWindow();

    FILE* temp1;
    FILE* temp2;
    FILE* temp3;
    freopen_s(&temp1,"CONOUT$", "w", stdout);
    freopen_s(&temp2,"CONOUT$", "w", stderr);
    freopen_s(&temp3,"CONIN$", "r", stdin);
    HMENU MenuHandle = GetSystemMenu((HWND)windowHandle, false);
    DeleteMenu(MenuHandle,SC_CLOSE, MF_BYCOMMAND);

    std::ios::sync_with_stdio(true);

    //Terminal setup
    ShowWindow((HWND)windowHandle,SW_SHOW);

    SMALL_RECT terminalSize; 
    //startup
    terminalSize.Left = 10;
    terminalSize.Top = 10;
    terminalSize.Right = 10;
    terminalSize.Bottom = 10;

    


    SetConsoleWindowInfo((HANDLE)outputHandle, true, &terminalSize);
    COORD terminalBuffer;
    terminalBuffer.X = 250;
    terminalBuffer.Y = 3000;
    SetConsoleScreenBufferSize((HANDLE)outputHandle,terminalBuffer);
    
    //real size
    terminalSize.Left = 0;
    terminalSize.Top = 0;
    terminalSize.Right = 149;
    terminalSize.Bottom = 29;
    SetConsoleWindowInfo((HANDLE)outputHandle, true, &terminalSize);

    

}


void Terminal::Clear(){


}

void Terminal::WriteArt(){

    SetConsoleOutputCP(65001);
    std::cout<<signature<<std::endl;
    SetConsoleTextAttribute((HWND)outputHandle, FOREGROUND_RED |FOREGROUND_INTENSITY);
    std::cout<<"F.T"<<std::endl;
    SetConsoleTextAttribute((HWND)outputHandle, FOREGROUND_BLUE |FOREGROUND_INTENSITY);
    std::cout<<"Terminal has initialized successfully!"<<std::endl;
    SetConsoleTextAttribute((HWND)outputHandle, FOREGROUND_GREEN|FOREGROUND_INTENSITY);
    std::cout<<"Build 0.3 (Unstable)"<<std::endl;
    SetConsoleTextAttribute((HWND)outputHandle, FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN);
    std::cout<<"===================================================================="<<std::endl;
}


Terminal::~Terminal(){

    FreeConsole();
}