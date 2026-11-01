
#pragma once

#include "Shader.h"
#include "Renderer.h"
#include <array>
struct Lane;
class Enemy;
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
    void renderEnemy(const Enemy& enemy, const Lane& lane, float animationTime);
    

private:

    Shader* spriteShader = nullptr;
    Renderer* renderer = nullptr;

    unsigned int meleeTexture = 0;
    unsigned int rangedTexture = 0;
    unsigned int defenseTexture = 0;
    std::array<std::array<unsigned int, 4>, 3> enemyMoveTextures{};
    bool initialized = false;
};
