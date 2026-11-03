#include "SoundSettings.h"
#include "stb_image.h"
#include "../systems/AudioManager.h"
void SoundSettings::increaseVolume() {
    masterVolume += 0.1f;

    if (masterVolume > 1.0f)
        masterVolume = 1.0f;

    if (audioManager)
        audioManager->setVolume(isMuted ? 0.0f : masterVolume);
}

void SoundSettings::decreaseVolume() {
    masterVolume -= 0.1f;

    if (masterVolume < 0.0f)
        masterVolume = 0.0f;

    if (audioManager)
        audioManager->setVolume(isMuted ? 0.0f : masterVolume);
}

void SoundSettings::toggleMute() {
    isMuted = !isMuted;

    if (audioManager)
        audioManager->setVolume(isMuted ? 0.0f : masterVolume);
}