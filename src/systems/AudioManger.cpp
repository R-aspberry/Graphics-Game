#define MINIAUDIO_IMPLEMENTATION
#include "AudioManager.h"

bool AudioManager::init() {
    ma_result result = ma_engine_init(nullptr, &engine);

    if (result != MA_SUCCESS) {
        return false;
    }

    initialized = true;
    return true;
}

void AudioManager::playBackgroundMusic() {
    if (!initialized)
        return;

    if (ma_sound_init_from_file(&engine, "assets/backgroundsound.wav",MA_SOUND_FLAG_STREAM,nullptr, nullptr,&backgroundMusic) != MA_SUCCESS)
        return;

    musicLoaded = true;

    ma_sound_set_looping(&backgroundMusic, MA_TRUE);
    ma_sound_start(&backgroundMusic);
   
}

void AudioManager::setVolume(float volume) {
    ma_engine_set_volume(&engine, volume);
}