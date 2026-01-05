#define MINIAUDIO_IMPLEMENTATION
#include "AudioManager.h"
#include "Development/Terminal.h"

AudioManager::AudioManager()
{
    ma_result result;
    result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS)
    {
	Terminal::Log(LOG_ERROR, "Failed to start Audio Engine (miniaudio.h)");
    } else
    {
	Terminal::Log(LOG_SUCCESS, "Audio Engine started successfully");
    }
}

AudioManager::~AudioManager()
{
    ma_engine_uninit(&engine);
}
void AudioManager::PlayOneShot(std::string name, float volume)
{
    Terminal::Log(LOG_INFO, "Audio request: " + name);
}
