
#define MINIAUDIO_IMPLEMENTATION
#include "SoundManager.h"
#include <algorithm>
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
        return false;

    initialized_ = true;
    applyVolume();

    return true;
}

bool SoundManager::playMusic(const char* path, bool loop)
{
    if (!initialized_)
        return false;

    stopMusic();

    ma_uint32 flags = MA_SOUND_FLAG_STREAM | MA_SOUND_FLAG_NO_SPATIALIZATION;
    if (ma_sound_init_from_file( &engine_, path, flags, nullptr, nullptr, &music_) != MA_SUCCESS)
        
        return false;

    musicLoaded_ = true;

    if (loop)
    {
        ma_sound_set_looping(&music_, MA_TRUE);
    }
    else
    {
        ma_sound_set_looping(&music_, MA_FALSE);
    }

    if (ma_sound_start(&music_) != MA_SUCCESS)
    {
        ma_sound_uninit(&music_);
        musicLoaded_ = false;
        return false;
    }

    return true;
}

bool SoundManager::playEffect(const char* path)
{
    return initialized_ && ma_engine_play_sound(&engine_, path, nullptr) == MA_SUCCESS;
}

void SoundManager::stopMusic()
{
    if (musicLoaded_)
    {
        ma_sound_uninit(&music_);
        musicLoaded_ = false;
    }
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
        if (muted_)
        {
            ma_engine_set_volume(&engine_, 0.0f);
        }
        else
        {
            ma_engine_set_volume(&engine_, volume_);
        }
    }
}

void SoundManager::shutdown()
{
    stopMusic();

    if (initialized_)
    {
        ma_engine_uninit(&engine_);
        initialized_ = false;
    }
}
