#pragma once
#include "miniaudio.h"

class AudioManager {
private:
    ma_engine engine{};
    bool initialized = false;
    ma_sound backgroundMusic{};
    bool musicLoaded = false;

public:
    bool init();
    void playBackgroundMusic();
    void shutdown();
    void setVolume(float volume);
};
