#pragma once
class AudioManager;
class SoundSettings {
private:
    float masterVolume = 0.5f;
    bool isMuted = false;
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int shaderProgram = 0;
    unsigned int backgroundTexture = 0;
    AudioManager* audioManager = nullptr;

public:
    void draw();
    void handleClick(double x, double y);

    void increaseVolume();
    void decreaseVolume();
    void toggleMute();
    void setAudioManager(AudioManager* audio);
};