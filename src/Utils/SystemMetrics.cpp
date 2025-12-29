#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <psapi.h>
#include "SystemMetrics.h"

namespace SystemMetrics {
float GetRAMUsage()
{
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
    {
	return static_cast<float>(pmc.WorkingSetSize) / (1024.0f * 1024.0f);
    }
    return 0.0f;
}

} // namespace SystemMetrics