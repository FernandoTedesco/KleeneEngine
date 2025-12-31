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
	return TempHistory;
    }
    int GetHistoryIndex() const
    {
	return HistoryOffset;
    }

private:
    void PollHardware();
    GpuSnapshot CurrentState;
    float PollTimer = 0.0f;
    const float PollInterval = 0.5f;
    float TempHistory[HISTORY_SIZE] = {0};
    float LoadHistory[HISTORY_SIZE] = {0};
    int HistoryOffset = 0;
};

namespace SystemMetrics {
float GetRAMUsage();
}