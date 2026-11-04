
#pragma once


    enum class SoundAction {
    None,
    VolumeDown,
    Mute,
    VolumeUp,
    Back
};


class SoundSettings {
public:
    void init();
    void draw();
    SoundAction handleClick(float x, float y);
    void cleanup();

private:
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int shaderProgram = 0;

    unsigned int volumeUpTexture = 0;
    unsigned int volumeDownTexture = 0;
    unsigned int muteTexture = 0;
    unsigned int backTexture = 0;
};
