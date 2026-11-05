#pragma once

#include <glad/gl.h>

class Cutscene
{
public:
    bool init();
    void start();
    void update(float deltaTime);
    void draw(int screenWidth, int screenHeight);
    bool finished() const;
    void stopAudio();
    void cleanup();

private:
    float time = 0.0f;
    bool ready = false;
    bool musicPlaying = false;

    GLuint shaderProgram = 0;
    GLuint VAO = 0;
    GLuint VBO = 0;

    GLuint background = 0;
    GLuint signal = 0;
    GLuint enemyText = 0;
    GLuint playerText = 0;
    GLuint title = 0;
    GLuint objective = 0;
    GLuint skipText = 0;

    GLuint playerWalk[4] = {};
    GLuint playerAttack[4] = {};
    GLuint enemyWalk[4] = {};
    GLuint enemyAttack[4] = {};
    GLuint enemyDeath[4] = {};

    void drawImage(GLuint texture, float x, float y,
                   float width, float height, float opacity = 1.0f,
                   bool flip = false);
};