#include "Terminal.h"
#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include "Editor.h"
unsigned int Terminal::ActiveFilters = LOG_ALL;
static const char* signature =
    R"(                                                                                        
  ▄▄▄▄   ▄▄▄ ▄▄                             ▄▄▄▄▄▄▄                                     
 █▀ ██  ██    ██                           █▀██▀▀▀                               ▄ ▄ ▄  
    ██ ██     ██             ▄               ██     ▄        ▄▄ ▀▀ ▄             ▄███▄  
    █████     ██ ▄█▀█▄ ▄█▀█▄ ████▄ ▄█▀█▄     ████   ████▄ ▄████ ██ ████▄ ▄█▀█▄   ▄▀█▀▄  
    ██ ██▄    ██ ██▄█▀ ██▄█▀ ██ ██ ██▄█▀     ██     ██ ██ ██ ██ ██ ██ ██ ██▄█▀          
  ▀██▀  ▀██▄ ▄██▄▀█▄▄▄▄▀█▄▄▄▄██ ▀█▄▀█▄▄▄     ▀█████▄██ ▀█▄▀████▄██▄██ ▀█▄▀█▄▄▄          
                                                             ██                         
                                                           ▀▀▀                          )";

Terminal::Terminal()
{
    FreeConsole();
    if (!AllocConsole())
    {
	if (GetLastError() != 5)
	{
	    return;
	}
    }
    // Terminal Creation
    outputHandle = (void*)GetStdHandle(STD_OUTPUT_HANDLE);
    windowHandle = (void*)GetConsoleWindow();
    inputHandle = (void*)GetStdHandle(STD_INPUT_HANDLE);
    FILE* temp1;
    FILE* temp2;
    FILE* temp3;
    freopen_s(&temp1, "CONOUT$", "w", stdout);
    freopen_s(&temp2, "CONOUT$", "w", stderr);
    freopen_s(&temp3, "CONIN$", "r", stdin);
    HMENU MenuHandle = GetSystemMenu((HWND)windowHandle, false);
    DeleteMenu(MenuHandle, SC_CLOSE, MF_BYCOMMAND);

    std::ios::sync_with_stdio(true);

    // Terminal setup
    ShowWindow((HWND)windowHandle, SW_SHOW);

    SMALL_RECT terminalSize;
    // startup
    terminalSize.Left = 10;
    terminalSize.Top = 10;
    terminalSize.Right = 10;
    terminalSize.Bottom = 10;

    SetConsoleWindowInfo((HANDLE)outputHandle, true, &terminalSize);
    COORD terminalBuffer;
    terminalBuffer.X = 250;
    terminalBuffer.Y = 3000;
    SetConsoleScreenBufferSize((HANDLE)outputHandle, terminalBuffer);

    // real size
    terminalSize.Left = 0;
    terminalSize.Top = 0;
    terminalSize.Right = 89;
    terminalSize.Bottom = 29;
    SetConsoleWindowInfo((HANDLE)outputHandle, true, &terminalSize);
}

unsigned long Terminal::UpdateConsoleInput()
{

    unsigned long bucket;
    GetNumberOfConsoleInputEvents(this->inputHandle, &bucket);
    return bucket;
}
void Terminal::ProcessConsoleInput()
{
    INPUT_RECORD recordBuffer;
    unsigned long eventsRead;
    ReadConsoleInput((HANDLE)this->inputHandle, &recordBuffer, 1, &eventsRead);
    if (recordBuffer.EventType == KEY_EVENT)
    {
	if (recordBuffer.Event.KeyEvent.bKeyDown)
	{
	    char character = recordBuffer.Event.KeyEvent.uChar.AsciiChar;
	    if (character == 8) // backspace
	    {
		if (!commandBuffer.empty())
		{
		    commandBuffer.pop_back();
		    std::cout << '\b' << ' ' << '\b';
		}
	    } else if (character == 13) // enter
	    {
		std::cout << std::endl;
		this->ExecuteConsoleCommand();
		commandBuffer.clear();
	    } else // anything else
	    {
		if (character >= 32 && character <= 126)
		{ // filter so its only ascii chars
		    commandBuffer += character;
		    std::cout << character;
		}
	    }
	}
    }
}
void Terminal::Log(LogCategory type, const std::string& message)
{
    if (!(ActiveFilters & type))
    {
	return;
    }
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    std::string prefix = "";
    WORD color = 0;
    switch (type)
    {
    case LOG_INFO:
	color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	prefix = "[INFO] ";
	break;
    case LOG_WARNING:
	color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	prefix = "[WARNING] ";
	break;
    case LOG_ERROR:
	color = FOREGROUND_RED | FOREGROUND_INTENSITY;
	prefix = "[ERROR] ";
	break;
    case LOG_RENDER:
	color = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	prefix = "[GFX] ";
	break;
    case LOG_CORE:
	color = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	prefix = "[CORE] ";
	break;
    default:
	color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
	prefix = "[LOG] ";
	break;
    }
    SetConsoleTextAttribute(hConsole, color);
    std::cout << prefix << message << std::endl;
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void Terminal::Clear()
{

    CONSOLE_SCREEN_BUFFER_INFO consoleScreenBufferInfo;

    GetConsoleScreenBufferInfo((HANDLE)outputHandle, &consoleScreenBufferInfo);
    unsigned long bufferArea = consoleScreenBufferInfo.dwSize.X * consoleScreenBufferInfo.dwSize.Y;
    unsigned long charsWritten;
    FillConsoleOutputCharacter(outputHandle, ' ', bufferArea, COORD{0, 0}, &charsWritten);
    SetConsoleCursorPosition(outputHandle, COORD{0, 0});
}

void Terminal::WriteArt()
{

    SetConsoleOutputCP(65001);
    std::cout << signature << std::endl;
    SetConsoleTextAttribute((HWND)outputHandle, FOREGROUND_RED | FOREGROUND_INTENSITY);
    std::cout << "F.T" << std::endl;
    SetConsoleTextAttribute((HWND)outputHandle, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    std::cout << "Terminal has initialized successfully!" << std::endl;
    SetConsoleTextAttribute((HWND)outputHandle, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    std::cout << "Build 0.8 (InDev)" << std::endl;
    SetConsoleTextAttribute((HWND)outputHandle,
			    FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN);
    std::cout << "================================================================================="
		 "========="
	      << std::endl;
}

void Terminal::ChangeLog()
{
    std::cout << "Changelog of build 0.9:" << std::endl;
    std::cout << "+Fog & Depth effects, simple depth of field " << std::endl;
    std::cout << "+Bloom & Color gradient" << std::endl;
    std::cout << "+miniaudio.h integration" << std::endl;
    std::cout << "+Particle System" << std::endl;
}

void Terminal::Help()
{
    std::cout << "Command List:" << std::endl;
    std::cout << "/help" << " - List all possible terminal commands" << std::endl;
    std::cout << "/art" << " - Print the terminal default message" << std::endl;
    std::cout << "/changelog" << " - Prints out the latest build changes" << std::endl;
    std::cout << "/wireframe (on/off)" << " - changes the entity rendering mode to wireframe"
	      << std::endl;
    std::cout << "/clear" << " - Clear the terminal buffer" << std::endl;
}

void Terminal::SetEditorContext(Editor* editor)
{
    this->editorContext = editor;
}
void Terminal::ExecuteConsoleCommand()
{
    if (commandBuffer == "/clear")
    {
	this->Clear();
    }
    if (commandBuffer == "/art")
    {
	this->WriteArt();
    }
    if (commandBuffer == "/help")
    {
	this->Help();
    }
    if (commandBuffer == "/changelog")
    {
	this->ChangeLog();
    }
    if (this->editorContext != nullptr)
    {
	if (commandBuffer == "/wireframe on")
	{
	    editorContext->debugWireframeMode = true;
	    std::cout << "[CMD] Wireframe Enabled." << std::endl;
	} else if (commandBuffer == "/wireframe off")
	{
	    editorContext->debugWireframeMode = false;
	    std::cout << "[CMD] Wireframe Disabled." << std::endl;
	} else if (commandBuffer == "/wireframe")
	{
	    editorContext->debugWireframeMode = !editorContext->debugWireframeMode;
	    std::cout << "[CMD] Wireframe Toggled." << std::endl;
	}

	if (commandBuffer == "/log render")
	{
	    ActiveFilters ^= LOG_RENDER;
	    Terminal::Log(LOG_INFO,
			  (ActiveFilters & LOG_RENDER) ? "Render Logs: ON" : "Render Logs: OFF");
	} else if (commandBuffer == "/log io")
	{
	    ActiveFilters ^= LOG_IO;
	    Terminal::Log(LOG_INFO, (ActiveFilters & LOG_IO) ? "IO Logs: ON" : "IO Logs: OFF");
	} else if (commandBuffer == "/log core")
	{
	    ActiveFilters ^= LOG_CORE;
	    Terminal::Log(LOG_INFO,
			  (ActiveFilters & LOG_CORE) ? "Core Logs: ON" : "Core Logs: OFF");
	} else if (commandBuffer == "/log all")
	{
	    ActiveFilters = LOG_ALL;
	    Terminal::Log(LOG_INFO, "Display ALL logs");
	} else if (commandBuffer == "/log error")
	{
	    ActiveFilters = LOG_ERROR;
	    Terminal::Log(LOG_ERROR, "Displaying ONLY errors");
	}
    }
}

Terminal::~Terminal()
{
    FreeConsole();
}