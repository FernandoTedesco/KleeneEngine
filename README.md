![Banner](assets/misc/Banner.png)


A game engine using OpenGL and mainly C++ 17. 

GLM is used for matrix math, IMGUI and SDL2 are used for the window setup.

## Target Architecture/Use info

Made for Windows 10/11 (x64)

CMake(3.14+)

MSVC Toolchain required

All the core dependencies located in deps/ (such as SDL2) are provided in the .lib format native to the Visual Studio Linker. For now this establishes MSVC as the primary compiler for this development phase. This will be adressed in the future though.