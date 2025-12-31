#pragma once
#include <vector>
#define MEMORY_HISTORY_SIZE 600
struct MemorySnapshot {
    float processRAM;
    float totalRAM;
    float totalSystemRAM;
};

class MemoryTracker
{
public:
    void Init();
    void Update(float deltaTime);
    const MemorySnapshot& GetCurrentState() const
    {
	return CurrentState;
    }

    const float* GetProcessHistory() const
    {
	return ProcessHistory;
    }
    const float* GetTotalHistory() const
    {
	return TotalHistory;
    }
    int GetHistoryIndex() const
    {
	return Offset;
    }

private:
    void PollMemory();

private:
    MemorySnapshot CurrentState;
    float ProcessHistory[MEMORY_HISTORY_SIZE] = {0};
    float TotalHistory[MEMORY_HISTORY_SIZE] = {0};
    int Offset = 0;
    float Timer = 0.0f;
    const float UpdateInterval = 0.1f;
};
