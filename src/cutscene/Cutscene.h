#pragma once

#include <glad/gl.h>
#include <array>

// Self-contained introductory cinematic. It never modifies the gameplay world.
// All graphics are OpenGL 3.3 and the art is loaded from existing project assets.
class Cutscene
{
public:
    static constexpr float Duration = 18.0f;

    bool init();
    void start();
    void update(float deltaTime);
    void draw(int framebufferWidth, int framebufferHeight);
    bool finished() const { return elapsed_ >= Duration; }
    void stopAudio();
    void cleanup(); // Must be called before destroying the OpenGL window.

private:
    float elapsed_ = 0.0f;
    bool initialized_ = false;
    bool audioPlaying_ = false;

    GLuint program_ = 0;
    GLuint vao_ = 0;
    GLuint vbo_ = 0;
    GLuint white_ = 0;
    GLuint portal_ = 0;

    GLuint city_ = 0;
    GLuint signal_ = 0;
    GLuint enemyLine_ = 0;
    GLuint heroLine_ = 0;
    GLuint title_ = 0;
    GLuint objective_ = 0;
    GLuint skip_ = 0;

    std::array<GLuint, 4> meleeWalk_{};
    std::array<GLuint, 4> meleeAttack_{};
    std::array<GLuint, 4> easyWalk_{};
    std::array<GLuint, 4> easyAttack_{};
    std::array<GLuint, 4> easyDeath_{};
    std::array<GLuint, 4> hardWalk_{};
    std::array<GLuint, 4> mediumWalk_{};

    void quad(GLuint texture, float x, float y, float w, float h,
              float r = 1.0f, float g = 1.0f, float b = 1.0f,
              float alpha = 1.0f, bool flipX = false,
              float glitch = 0.0f, float uCrop = 0.0f);
    void box(float x, float y, float w, float h,
             float r, float g, float b, float alpha);
    void glow(float cx, float cy, float width, float height,
              float r, float g, float b, float alpha);
    void atmosphere(float intensity);
};
