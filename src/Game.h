
#pragma once

#include "Shader.h"
#include "Renderer.h"
#include "systems/WaveManager.h"
struct Lane;
class Game
{
public:
    Game() = default;
    ~Game();

    void init();
    void update(float deltaTime);
    void render(const Lane& lane);

    // WaveManager owns the enemies; the game updates it once per gameplay frame.
    WaveManager& getWaveManager() { return waves; }
    const WaveManager& getWaveManager() const { return waves; }

private:
    Shader* spriteShader = nullptr;
    Renderer* renderer = nullptr;

    unsigned int meleeTexture = 0;
    bool initialized = false;
    WaveManager waves;
    bool victoryReported = false;
};
