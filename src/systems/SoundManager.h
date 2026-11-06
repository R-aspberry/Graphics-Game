
#pragma once

#include "../../external/miniaudio/miniaudio.h"

class SoundManager
{
public:
    static SoundManager& instance();

    bool init();
    bool playMusic(const char* path, bool loop = true);
    bool playEffect(const char* path);
    void stopMusic();

    void setVolume(float value);
    float getVolume() const;

    void toggleMute();
    bool isMuted() const;

    void shutdown();

private:
    SoundManager() = default;
    ~SoundManager();

    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;

    void applyVolume();

    ma_engine engine_{};
    ma_sound music_{};

    bool initialized_ = false;
    bool musicLoaded_ = false;
    bool muted_ = false;

    float volume_ = 0.7f;
};
