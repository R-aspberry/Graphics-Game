
#ifndef GAME_H
#define GAME_H

#include "Shader.h"
#include "Renderer.h"
#include "systems/WaveManager.h"
#include <vector>
#include "world/Lane.h"

#include <array>
#include "world/Lane.h"

struct Lane;
class Game

{

public:
    enum class player{
    MELEE,
    RANGED,
    DEFENSE};
    Game() = default;
    ~Game();

    void init();
    void update(float deltaTime);
    void render(const Lane& lane, player type);
    
//     void render(const std::array<Lane,4>& lane);

//     // WaveManager owns the enemies; the game updates it once per gameplay frame.
//     WaveManager& getWaveManager() { return waves; }
//     const WaveManager& getWaveManager() const { return waves; }

private:

    Shader* spriteShader = nullptr;
    Renderer* renderer = nullptr;

    unsigned int meleeTexture = 0;
    unsigned int rangedTexture = 0;
    unsigned int defenseTexture = 0;

    bool initialized = false;
    WaveManager waves;
    bool victoryReported = false;
};

#endif