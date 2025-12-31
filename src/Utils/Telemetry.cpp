#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <psapi.h>
#include <iostream>
#include "Telemetry.h"
#include "nvml.h"

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

using ptr_nvmlInit = decltype(&nvmlInit);
using ptr_nvmlShutdown = decltype(&nvmlShutdown);
using ptr_nvmlDeviceGetHandleByIndex = decltype(&nvmlDeviceGetHandleByIndex);
using ptr_nvmlDeviceGetTemperature = decltype(&nvmlDeviceGetTemperature);
using ptr_nvmlDeviceGetUtilizationRates = decltype(&nvmlDeviceGetUtilizationRates);
using ptr_nvmlDeviceGetFanSpeed = decltype(&nvmlDeviceGetFanSpeed);
using ptr_nvmlDeviceGetPowerUsage = decltype(&nvmlDeviceGetPowerUsage);

static HMODULE g_NvmlDll = nullptr;
static nvmlDevice_t g_GpuHandle = nullptr;
static bool g_IsNvidiaInitialized = false;

static ptr_nvmlInit _nvmlInit = nullptr;
static ptr_nvmlShutdown _nvmlShutdown = nullptr;
static ptr_nvmlDeviceGetHandleByIndex _nvmlDeviceGetHandleByIndex = nullptr;
static ptr_nvmlDeviceGetTemperature _nvmlDeviceGetTemperature = nullptr;
static ptr_nvmlDeviceGetUtilizationRates _nvmlDeviceGetUtilizationRates = nullptr;
static ptr_nvmlDeviceGetFanSpeed _nvmlDeviceGetFanSpeed = nullptr;
static ptr_nvmlDeviceGetPowerUsage _nvmlDeviceGetPowerUsage = nullptr;

void GpuTelemetry::Init()
{
    g_NvmlDll = LoadLibraryA("nvml.dll");
    if (!g_NvmlDll)
    {
	std::cout << "[ERROR] nvml.dll not found in the system." << std::endl;
	return;
    }
    _nvmlInit = (ptr_nvmlInit)GetProcAddress(g_NvmlDll, "nvmlInit_v2");
    _nvmlShutdown = (ptr_nvmlShutdown)GetProcAddress(g_NvmlDll, "nvmlShutdown");

    _nvmlDeviceGetHandleByIndex =
	(ptr_nvmlDeviceGetHandleByIndex)GetProcAddress(g_NvmlDll, "nvmlDeviceGetHandleByIndex_v2");

    _nvmlDeviceGetTemperature =
	(ptr_nvmlDeviceGetTemperature)GetProcAddress(g_NvmlDll, "nvmlDeviceGetTemperature");
    _nvmlDeviceGetUtilizationRates = (ptr_nvmlDeviceGetUtilizationRates)GetProcAddress(
	g_NvmlDll, "nvmlDeviceGetUtilizationRates");
    _nvmlDeviceGetFanSpeed =
	(ptr_nvmlDeviceGetFanSpeed)GetProcAddress(g_NvmlDll, "nvmlDeviceGetFanSpeed");
    _nvmlDeviceGetPowerUsage =
	(ptr_nvmlDeviceGetPowerUsage)GetProcAddress(g_NvmlDll, "nvmlDeviceGetPowerUsage");

    if (_nvmlInit && _nvmlInit() == NVML_SUCCESS)
    {
	if (_nvmlDeviceGetHandleByIndex &&
	    _nvmlDeviceGetHandleByIndex(0, &g_GpuHandle) == NVML_SUCCESS)
	{
	    g_IsNvidiaInitialized = true;
	    std::cout << "[INFO] NVIDIA GPU Ready" << std::endl;
	}
    }
}

void GpuTelemetry::Shutdown()
{
    if (g_IsNvidiaInitialized && _nvmlShutdown)
    {
	_nvmlShutdown();
    }
    if (g_NvmlDll)
    {
	FreeLibrary(g_NvmlDll);
	g_NvmlDll = nullptr;
    }
    g_IsNvidiaInitialized = false;
}

void GpuTelemetry::Update(float deltaTime)
{
    if (!g_IsNvidiaInitialized)
	return;
    PollTimer += deltaTime;
    if (PollTimer >= PollInterval)
    {
	PollTimer = 0.0f;
	PollHardware();
    }
}

void GpuTelemetry::PollHardware()
{
    if (!g_IsNvidiaInitialized)
	return;
    unsigned int temperature = 0;
    if (_nvmlDeviceGetTemperature(g_GpuHandle, NVML_TEMPERATURE_GPU, &temperature) == NVML_SUCCESS)
    {
	CurrentState.temperature = (float)temperature;
    }
    nvmlUtilization_t rates;
    if (_nvmlDeviceGetUtilizationRates(g_GpuHandle, &rates) == NVML_SUCCESS)
    {
	CurrentState.loadCore = (float)rates.gpu;
	CurrentState.loadMemory = (float)rates.memory;
    }
    unsigned int fan = 0;
    if (_nvmlDeviceGetFanSpeed && _nvmlDeviceGetFanSpeed(g_GpuHandle, &fan) == NVML_SUCCESS)
    {
	CurrentState.fanSpeed = (float)fan;
    }
    unsigned int power = 0;
    if (_nvmlDeviceGetPowerUsage && _nvmlDeviceGetPowerUsage(g_GpuHandle, &power) == NVML_SUCCESS)
    {
	CurrentState.powerWatts = power / 1000.0f;
    }
    TempHistory[HistoryOffset] = CurrentState.temperature;
    LoadHistory[HistoryOffset] = CurrentState.loadCore;
    HistoryOffset = (HistoryOffset + 1) % HISTORY_SIZE;
}
