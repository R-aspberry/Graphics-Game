
#define MINIAUDIO_IMPLEMENTATION
#include "SoundManager.h"
#define MINIAUDIO_IMPLEMENTATION
#include "SoundManager.h"

#include <algorithm>

#include <algorithm>
#include <iostream>

#define MINIAUDIO_IMPLEMENTATION
#include "SoundManager.h"

#include <iostream>

SoundManager& SoundManager::instance()
{
    static SoundManager manager;
    return manager;
}

SoundManager::~SoundManager()
{
    shutdown();
}

bool SoundManager::init()
{
    if (initialized_)
        return true;

    if (ma_engine_init(nullptr, &engine_) != MA_SUCCESS)
    {
        std::cerr << "Failed to initialize audio\n";
        return false;
    }

    initialized_ = true;
    applyVolume();

    return true;
}

bool SoundManager::playMusic(const char* path, bool loop)
{
    if (!initialized_)
        return false;

    if (musicLoaded_)
    {
        ma_sound_uninit(&music_);
        musicLoaded_ = false;
    }

    ma_uint32 flags =
        MA_SOUND_FLAG_STREAM |
        MA_SOUND_FLAG_NO_SPATIALIZATION;

    if (ma_sound_init_from_file(
            &engine_,
            path,
            flags,
            nullptr,
            nullptr,
            &music_) != MA_SUCCESS)
    {
        std::cerr << "Failed to load music: "
                  << path << '\n';
        return false;
    }

    musicLoaded_ = true;

    ma_sound_set_looping(
        &music_,
        loop ? MA_TRUE : MA_FALSE
    );

    if (ma_sound_start(&music_) != MA_SUCCESS)
    {
        ma_sound_uninit(&music_);
        musicLoaded_ = false;
        return false;
    }

    return true;
}

void SoundManager::setVolume(float value)
{
    // Keep volume between 0.0 and 1.0
    if (value < 0.0f)
        value = 0.0f;

    if (value > 1.0f)
        value = 1.0f;

    volume_ = value;
    applyVolume();
}

float SoundManager::getVolume() const
{
    return volume_;
}

void SoundManager::toggleMute()
{
    muted_ = !muted_;
    applyVolume();
}

bool SoundManager::isMuted() const
{
    return muted_;
}

void SoundManager::applyVolume()
{
    if (initialized_)
    {
        ma_engine_set_volume(
            &engine_,
            muted_ ? 0.0f : volume_
        );
    }
}

void SoundManager::shutdown()
{
    if (musicLoaded_)
    {
        ma_sound_uninit(&music_);
        musicLoaded_ = false;
    }

    if (initialized_)
    {
        ma_engine_uninit(&engine_);
        initialized_ = false;
    }
}
