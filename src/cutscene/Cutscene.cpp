#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "Cutscene.h"
#include "utils/ResourceManager.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#endif

namespace
{
    const char *kVertex = R"(
#version 330 core
layout(location=0) in vec2 aPos;
layout(location=1) in vec2 aUV;
out vec2 uv;
uniform vec4 rect;
uniform vec4 uvRect;
void main() {
    gl_Position = vec4(rect.xy + rect.zw*aPos, 0.0, 1.0);
    uv = uvRect.xy + aUV*uvRect.zw;
}
)";
    const char *kFragment = R"(
#version 330 core
in vec2 uv;
out vec4 FragColor;
uniform sampler2D image;
uniform vec4 tint;
uniform float time;
uniform float glitch;
void main() {
    float strip = floor(uv.y*125.0);
    float jolt = step(0.80, fract(sin(strip*65.73 + floor(time*18.0)*23.37)*4358.23));
    vec2 warped = uv + vec2(jolt*glitch*(sin(time*7.0)>0.0?1.0:-1.0), 0.0);
    vec4 tex = texture(image, warped);
    if (glitch > 0.001) {
        tex.r = texture(image, warped + vec2(0.004*glitch,0.0)).r;
        tex.b = texture(image, warped - vec2(0.004*glitch,0.0)).b;
    }
    // A subtle analog scanline, not a flashing strobe.
    float scan = 1.0 - 0.045*(0.5+0.5*sin(gl_FragCoord.y*1.55));
    FragColor = vec4(tex.rgb*tint.rgb*scan, tex.a*tint.a);
}
)";

    float clamp01(float x) { return std::max(0.0f, std::min(1.0f, x)); }
    float smooth(float x)
    {
        x = clamp01(x);
        return x * x * (3.0f - 2.0f * x);
    }
    float ramp(float t, float a, float b) { return smooth((t - a) / (b - a)); }
    float window(float t, float in0, float in1, float out0, float out1)
    {
        return ramp(t, in0, in1) * (1.0f - ramp(t, out0, out1));
    }
    float pseudo(float n) { return std::sin(n * 12.9898f) * 43758.5453f - std::floor(std::sin(n * 12.9898f) * 43758.5453f); }

    GLuint compile(GLenum kind, const char *source)
    {
        GLuint s = glCreateShader(kind);
        glShaderSource(s, 1, &source, nullptr);
        glCompileShader(s);
        GLint okay = GL_FALSE;
        glGetShaderiv(s, GL_COMPILE_STATUS, &okay);
        if (!okay)
        {
            char error[2048] = {};
            glGetShaderInfoLog(s, sizeof(error), nullptr, error);
            std::cerr << "Cutscene shader compilation failed: " << error << '\n';
            glDeleteShader(s);
            return 0;
        }
        return s;
    }

    GLuint uploadRGBA(int w, int h, const std::vector<unsigned char> &pixels)
    {
        GLuint texture = 0;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);
        return texture;
    }

    void loadFrames(std::array<GLuint, 4> &frames, const std::string &folder,
                    const std::string &prefix, const std::string &action)
    {
        for (int i = 0; i < 4; ++i)
            frames[i] = ResourceManager::loadTexture(folder + prefix + "_" + action + std::to_string(i + 1) + ".png");
    }
} // namespace

bool Cutscene::init()
{
    if (initialized_)
        return true;
    GLuint vertex = compile(GL_VERTEX_SHADER, kVertex);
    GLuint fragment = compile(GL_FRAGMENT_SHADER, kFragment);
    if (!vertex || !fragment)
    {
        if (vertex)
            glDeleteShader(vertex);
        if (fragment)
            glDeleteShader(fragment);
        return false;
    }
    program_ = glCreateProgram();
    glAttachShader(program_, vertex);
    glAttachShader(program_, fragment);
    glLinkProgram(program_);
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    GLint okay = GL_FALSE;
    glGetProgramiv(program_, GL_LINK_STATUS, &okay);
    if (!okay)
    {
        char error[2048] = {};
        glGetProgramInfoLog(program_, sizeof(error), nullptr, error);
        std::cerr << "Cutscene shader link failed: " << error << '\n';
        cleanup();
        return false;
    }

    // Local unit quad, bottom-left origin, full alpha RGBA sprite rendering.
    const float quadVertices[] = {
        0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1,
        0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1};
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void *>(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    white_ = uploadRGBA(1, 1, {255, 255, 255, 255});
    std::vector<unsigned char> portalPixels(128 * 128 * 4);
    for (int y = 0; y < 128; ++y)
        for (int x = 0; x < 128; ++x)
        {
            float dx = (x - 63.5f) / 63.5f, dy = (y - 63.5f) / 63.5f;
            float dist = std::sqrt(dx * dx + dy * dy);
            float ring = std::exp(-std::pow((dist - 0.62f) / 0.10f, 2.0f));
            float core = std::exp(-dist * dist * 8.0f);
            float aura = std::exp(-dist * dist * 1.7f);
            float alpha = clamp01(0.95f * ring + 0.27f * core + 0.20f * aura);
            const size_t offset = static_cast<size_t>((y * 128 + x) * 4);
            portalPixels[offset + 0] = 210;
            portalPixels[offset + 1] = 92;
            portalPixels[offset + 2] = 255;
            portalPixels[offset + 3] = static_cast<unsigned char>(alpha * 255.0f);
        }
    portal_ = uploadRGBA(128, 128, portalPixels);

    city_ = ResourceManager::loadTexture("assets/backgrounds/cyberpunk.png");
    signal_ = ResourceManager::loadTexture("assets/cutscene/signal.png");
    enemyLine_ = ResourceManager::loadTexture("assets/cutscene/enemy_line.png");
    heroLine_ = ResourceManager::loadTexture("assets/cutscene/hero_line.png");
    title_ = ResourceManager::loadTexture("assets/cutscene/title.png");
    objective_ = ResourceManager::loadTexture("assets/cutscene/objective.png");
    skip_ = ResourceManager::loadTexture("assets/cutscene/skip.png");

    loadFrames(meleeWalk_, "assets/sprites/", "Melee", "M");
    loadFrames(meleeAttack_, "assets/sprites/", "Melee", "A");
    loadFrames(easyWalk_, "assets/sprites/enemies/EASYENEMY/", "EASYENEMY", "M");
    loadFrames(easyAttack_, "assets/sprites/enemies/EASYENEMY/", "EASYENEMY", "A");
    loadFrames(easyDeath_, "assets/sprites/enemies/EASYENEMY/", "EASYENEMY", "D");
    loadFrames(mediumWalk_, "assets/sprites/enemies/MEDIUMENEMY/", "MEDIUMENEMY", "M");
    loadFrames(hardWalk_, "assets/sprites/enemies/HARDENEMY/", "HARDENEMY", "M");

    initialized_ = program_ && vao_ && city_ && meleeWalk_[0] && easyWalk_[0] && signal_ && enemyLine_ && heroLine_ && title_ && objective_;
    if (!initialized_)
        std::cerr << "Cutscene: one or more assets could not be loaded. Check paths and launch from project root.\n";
    return initialized_;
}

void Cutscene::start()
{
    elapsed_ = 0.0f;
#ifdef _WIN32
    // Plays original 18-second cinematic cue; temporarily replaces the menu loop.
    audioPlaying_ = PlaySoundA("assets/cutscene/cinematic.wav", nullptr,
                               SND_ASYNC | SND_FILENAME) != FALSE;
#endif
}

void Cutscene::update(float deltaTime) { elapsed_ = std::min(Duration, elapsed_ + std::max(0.0f, deltaTime)); }

void Cutscene::stopAudio()
{
#ifdef _WIN32
    if (audioPlaying_)
    {
        PlaySoundA("assets/backgroundsound.wav", nullptr, SND_ASYNC | SND_LOOP | SND_FILENAME);
        audioPlaying_ = false;
    }
#endif
}

void Cutscene::quad(GLuint texture, float x, float y, float w, float h,
                    float r, float g, float b, float alpha, bool flipX, float glitch, float uCrop)
{
    if (!texture || alpha <= 0.001f || !program_)
        return;
    glUseProgram(program_);
    glUniform4f(glGetUniformLocation(program_, "rect"), x, y, w, h);
    glUniform4f(glGetUniformLocation(program_, "uvRect"),
                flipX ? (1.0f - uCrop * 0.5f) : (uCrop * 0.5f), 0.0f,
                flipX ? -(1.0f - uCrop) : (1.0f - uCrop), 1.0f);
    glUniform4f(glGetUniformLocation(program_, "tint"), r, g, b, clamp01(alpha));
    glUniform1f(glGetUniformLocation(program_, "time"), elapsed_);
    glUniform1f(glGetUniformLocation(program_, "glitch"), glitch);
    glUniform1i(glGetUniformLocation(program_, "image"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
void Cutscene::box(float x, float y, float w, float h, float r, float g, float b, float a)
{
    quad(white_, x, y, w, h, r, g, b, a);
}
void Cutscene::glow(float cx, float cy, float width, float height, float r, float g, float b, float a)
{
    quad(portal_, cx - width * 0.5f, cy - height * 0.5f, width, height, r, g, b, a);
}
void Cutscene::atmosphere(float intensity)
{
    // Procedural neon rain: repeatable streaks, no assets and no RNG state.
    for (int i = 0; i < 66; ++i)
    {
        float x = -1.0f + 2.0f * pseudo(i + 3.1f);
        float y = 1.2f - 2.4f * std::fmod(pseudo(i + 8.4f) + elapsed_ * (0.11f + pseudo(i + 11.2f) * 0.13f), 1.0f);
        float length = 0.018f + 0.045f * pseudo(i + 22.4f);
        float alpha = intensity * (0.09f + 0.14f * pseudo(i + 6.8f));
        box(x, y, 0.0014f, length, 0.35f, 0.9f, 1.0f, alpha);
    }
    // Dynamic scan/signal guides; they feel like a compromised HUD.
    float scanY = 0.78f - 1.7f * std::fmod(elapsed_ * 0.16f, 1.0f);
    box(-1.0f, scanY, 2.0f, 0.004f, 0.22f, 1.0f, 1.0f, 0.11f * intensity);
}

void Cutscene::draw(int framebufferWidth, int framebufferHeight)
{
    if (!initialized_ || framebufferWidth <= 0 || framebufferHeight <= 0)
        return;
    glViewport(0, 0, framebufferWidth, framebufferHeight);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_SCISSOR_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.011f, 0.013f, 0.045f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    float t = elapsed_;
    float shake = (t > 10.7f && t < 12.1f) ? std::sin(t * 125.0f) * 0.009f * window(t, 10.7f, 10.9f, 11.75f, 12.1f) : 0.0f;
    float bgZoom = 0.015f + 0.030f * ramp(t, 0.0f, 14.0f);
    float bgReveal = ramp(t, 0.10f, 1.20f);
    // Center-crop the very wide city image so it remains cinematic at 1000x700.
    // Unlike stretching it, cropping preserves the skyscraper and neon proportions.
    const float sourceAspect = 1967.0f / 799.0f;
    const float screenAspect = static_cast<float>(framebufferWidth) / framebufferHeight;
    const float crop = 1.0f - std::min(1.0f, screenAspect / sourceAspect);
    quad(city_, -1.0f - bgZoom + shake, -1.0f - bgZoom, 2.0f + 2 * bgZoom, 2.0f + 2 * bgZoom,
         0.57f, 0.69f, 0.94f, bgReveal, false,
         window(t, 2.7f, 2.85f, 3.20f, 3.4f) * 0.09f + window(t, 11.1f, 11.15f, 11.55f, 11.8f) * 0.05f,
         crop);
    box(-1.0f, -1.0f, 2.0f, 2.0f, 0.02f, 0.01f, 0.08f, 0.22f);
    atmosphere(bgReveal);

    // Hazard HUD: an urgent transmission arriving before the threat.
    float signalA = window(t, 0.5f, 1.2f, 3.0f, 3.65f);
    box(-0.91f, 0.68f, 1.23f, 0.004f, 0.12f, 0.95f, 1.0f, signalA * 0.66f);
    quad(signal_, -0.89f, 0.49f, 1.35f, 0.32f, 1, 1, 1, signalA);
    for (int i = 0; i < 4; ++i)
    {
        float a = signalA * (0.15f + 0.15f * std::sin(t * 3.0f + i));
        box(-0.87f + i * 0.16f, 0.43f, 0.12f, 0.016f, 0.16f, 1.0f, 1.0f, a);
    }

    const float portalA = window(t, 3.0f, 4.0f, 13.0f, 14.9f);
    const float portalPulse = 1.0f + 0.07f * std::sin(t * 11.0f);
    glow(0.74f + shake, -0.17f, 0.83f * portalPulse, 1.33f * portalPulse,
         0.94f, 0.32f, 1.0f, portalA * 0.94f);
    if (portalA > 0.001f)
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE); // bloom-like additive energy.
        for (int i = 0; i < 18; ++i)
        {
            float ang = i * 0.349f + t * (0.34f + (i % 3) * 0.13f);
            float radius = 0.25f + 0.05f * std::sin(t * 5.0f + i);
            float xx = 0.73f + radius * std::cos(ang);
            float yy = -0.18f + radius * 1.4f * std::sin(ang);
            box(xx, yy, 0.008f, 0.030f, 0.77f, 0.24f, 1.0f, 0.40f * portalA);
        }
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    float enemyA = window(t, 3.45f, 4.08f, 13.5f, 14.4f);

    float enemyAdvance = clamp01((t - 3.6f) / 6.4f);

    float enemyX = 0.93f - 0.65f * enemyAdvance - 0.22f * ramp(t, 10.0f, 11.1f) + shake;

    int moveFrame = static_cast<int>(t * 7.0f) % 4;
    int atkFrame = static_cast<int>((t - 10.0f) * 8.0f) % 4;
    if (atkFrame < 0)
        atkFrame = 0;
    GLuint enemyTex = (t >= 10.1f && t < 13.35f) ? easyAttack_[atkFrame] : easyWalk_[moveFrame];
    glow(enemyX + 0.23f, -0.69f, 0.5f, 0.23f, 0.72f, 0.09f, 1.0f, enemyA * 0.43f);
    quad(enemyTex, enemyX, -0.68f, 0.48f, 0.82f, 1, 1, 1, enemyA, true);
    // The harder enemies loom behind the first invader.
    if (t >= 4.5f && t < 9.7f)
    {
        float rearA = window(t, 4.5f, 5.3f, 8.5f, 9.7f) * 0.40f;
        quad(hardWalk_[moveFrame], 0.56f, -0.58f, 0.37f, 0.73f, 0.65f, 0.42f, 0.81f, rearA, true);
        quad(mediumWalk_[moveFrame], 0.72f, 0.15f + 0.05f * std::sin(t * 4.0f), 0.28f, 0.49f, 0.70f, 0.55f, 0.95f, rearA);
    }
    float enemyLineA = window(t, 4.2f, 4.6f, 6.1f, 6.9f);
    quad(enemyLine_, -0.80f, -0.90f, 1.60f, 0.31f, 1, 1, 1, enemyLineA);

    float heroA = window(t, 6.0f, 6.45f, 15.0f, 16.25f);
    float heroAdvance = ramp(t, 6.2f, 9.5f);
    float heroX = -1.05f + 0.53f * heroAdvance + 0.30f * ramp(t, 10.0f, 11.1f) + shake;
    GLuint heroTex = (t >= 10.1f && t < 13.35f) ? meleeAttack_[atkFrame] : meleeWalk_[moveFrame];
    glow(heroX + 0.25f, -0.7f, 0.66f, 0.25f, 0.12f, 1.0f, 0.94f, heroA * 0.52f);
    quad(heroTex, heroX, -0.68f, 0.53f, 0.99f, 1, 1, 1, heroA);
    float heroLineA = window(t, 7.2f, 7.7f, 9.35f, 10.1f);
    quad(heroLine_, -0.80f, -0.90f, 1.60f, 0.31f, 1, 1, 1, heroLineA);

    // One readable impact, a sword-vs-robot strike with a brief screen shake.
    float impactA = window(t, 10.7f, 10.95f, 11.45f, 12.10f);
    glow(0.02f + shake, -0.18f, 1.09f, 1.28f, 0.95f, 0.30f, 1.0f, impactA);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    for (int i = 0; i < 10; ++i)
    {
        float theta = i * 0.63f + t * 3.0f;
        float len = 0.15f + 0.33f * impactA;
        box(0.01f + std::cos(theta) * len * 0.65f, -0.17f + std::sin(theta) * len * 0.65f,
            0.008f, 0.045f, 0.45f, 0.94f, 1.0f, impactA * 0.68f);
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    float singleFlash = window(t, 11.18f, 11.24f, 11.30f, 11.48f);
    box(-1.0f, -1.0f, 2.0f, 2.0f, 0.83f, 0.98f, 1.0f, singleFlash * 0.78f);

    float deathA = window(t, 13.18f, 13.38f, 14.45f, 15.2f);
    int deathFrame = std::min(3, std::max(0, static_cast<int>((t - 13.3f) * 6.5f)));
    quad(easyDeath_[deathFrame], 0.25f, -0.68f, 0.50f, 0.83f, 1, 1, 1, deathA, true);

    float titleA = window(t, 14.7f, 15.45f, 17.2f, 18.0f);
    box(-1.0f, -1.0f, 2.0f, 2.0f, 0.01f, 0.02f, 0.055f, titleA * 0.88f);
    glow(0.0f, 0.05f, 1.65f, 1.1f, 0.16f, 0.75f, 1.0f, titleA * 0.30f);
    quad(title_, -0.86f, -0.09f, 1.72f, 0.57f, 1, 1, 1, titleA);
    quad(objective_, -0.78f, -0.52f, 1.56f, 0.28f, 1, 1, 1, titleA * ramp(t, 15.6f, 16.4f));

    // Letterbox and minimalist cinematic HUD stay over everything.
    box(-1.0f, 0.80f, 2.0f, 0.20f, 0.01f, 0.013f, 0.035f, 1.0f);
    box(-1.0f, -1.0f, 2.0f, 0.17f, 0.01f, 0.013f, 0.035f, 1.0f);
    box(-1.0f, 0.795f, 2.0f, 0.004f, 0.23f, 0.85f, 1.0f, 0.45f);
    box(-1.0f, -0.835f, 2.0f, 0.004f, 0.72f, 0.20f, 1.0f, 0.40f);
    float initialFade = 1.0f - ramp(t, 0.0f, 0.75f);
    float finalFade = ramp(t, 17.4f, Duration);
    box(-1, -1, 2, 2, 0.0f, 0.0f, 0.018f, std::max(initialFade, finalFade));
    if (skip_)
        quad(skip_, 0.36f, -0.969f, 0.59f, 0.13f, 1, 1, 1, 0.55f * (1.0f - finalFade));
    glDisable(GL_BLEND);
    glBindVertexArray(0);
    glUseProgram(0);
}

void Cutscene::cleanup()
{
    stopAudio();
    if (vbo_)
    {
        glDeleteBuffers(1, &vbo_);
        vbo_ = 0;
    }
    if (vao_)
    {
        glDeleteVertexArrays(1, &vao_);
        vao_ = 0;
    }
    if (white_)
    {
        glDeleteTextures(1, &white_);
        white_ = 0;
    }
    if (portal_)
    {
        glDeleteTextures(1, &portal_);
        portal_ = 0;
    }
    if (program_)
    {
        glDeleteProgram(program_);
        program_ = 0;
    }
    initialized_ = false;
}
