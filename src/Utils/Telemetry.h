#pragma once
#include <vector>
#define HISTORY_SIZE 600
struct GpuSnapshot {
    float loadCore;
    float loadMemory;
    float temperature;
    float fanSpeed;
    float powerWatts;
};

class GpuTelemetry
{
public:
    void Init();
    void Update(float deltaTime);
    void Shutdown();
    const GpuSnapshot& GetCurrentState() const
    {
	return CurrentState;
    }
    const float* GetTemperatureHistory() const
    {
	return TemperatureHistory;
    }
    const float* GetCoreLoadHistory() const
    {
	return CoreLoadHistory;
    }
    int GetHistoryIndex() const
    {
	return HistoryOffset;
    }
    const float* GetVramHistory() const
    {
	return VramLoadHistory;
    }
    const float* GetFanHistory() const
    {
	return FanHistory;
    }
    const float* GetPowerHistory() const
    {
	return PowerHistory;
    }

private:
    void PollHardware();
    GpuSnapshot CurrentState;
    float PollTimer = 0.0f;
    const float PollInterval = 0.5f;
    float TemperatureHistory[HISTORY_SIZE] = {0};
    float CoreLoadHistory[HISTORY_SIZE] = {0};
    float VramLoadHistory[HISTORY_SIZE] = {0};
    float FanHistory[HISTORY_SIZE] = {0};
    float PowerHistory[HISTORY_SIZE] = {0};
    int HistoryOffset = 0;
};

namespace SystemMetrics {
float GetRAMUsage();
};