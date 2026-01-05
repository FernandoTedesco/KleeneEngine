#pragma once
#include <miniaudio.h>
#include <glm/glm.hpp>
#include <string>

class AudioManager
{
public:
    AudioManager();
    ~AudioManager();
    void PlayOneShot(std::string name, float volume = 1.0f);

private:
    ma_engine engine;
};