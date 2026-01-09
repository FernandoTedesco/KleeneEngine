#include "Core/Engine.h"
#include "Core/Paths.h"
int main(int argc, char* argv[])
{
    Paths::Init();
    Engine kleeneEngine;
    kleeneEngine.Run();

    return 0;
}