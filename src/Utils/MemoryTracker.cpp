#include "MemoryTracker.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <psapi.h>
#include <iostream>
#pragma comment(lib, "psapi.lib")

void MemoryTracker::Init()
{
    MEMORYSTATUSEX memoryInfo;
    memoryInfo.dwLength = sizeof(MEMORYSTATUSEX);
    if (GlobalMemoryStatusEx(&memoryInfo))
    {
	CurrentState.totalSystemRAM = (float)memoryInfo.ullTotalPhys / (1024.0f * 1024.0f);
	std::cout << "[INFO] Memory Tracker initialized, Total System RAM:"
		  << CurrentState.totalSystemRAM << "MB" << std::endl;
    }
}
void MemoryTracker::Update(float deltaTime)
{
    Timer += deltaTime;
    if (Timer >= UpdateInterval)
    {
	Timer = 0.0f;
	PollMemory();
    }
}
void MemoryTracker::PollMemory()
{
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
    {
	CurrentState.processRAM = (float)pmc.WorkingSetSize / (1024.0f * 1024.0f);
    }
    MEMORYSTATUSEX memoryInfo;
    memoryInfo.dwLength = sizeof(MEMORYSTATUSEX);
    if (GlobalMemoryStatusEx(&memoryInfo))
    {
	DWORDLONG usedBytes = memoryInfo.ullTotalPhys - memoryInfo.ullAvailPhys;
	CurrentState.totalRAM = (float)usedBytes / (1024.0f * 1024.0f);
    }
    ProcessHistory[Offset] = CurrentState.processRAM;
    TotalHistory[Offset] = CurrentState.totalRAM;

    Offset = (Offset + 1) % MEMORY_HISTORY_SIZE;
}