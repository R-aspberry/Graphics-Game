
#pragma once

#include "Shader.h"
#include "Renderer.h"
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
    

private:

    Shader* spriteShader = nullptr;
    Renderer* renderer = nullptr;

    unsigned int meleeTexture = 0;
    unsigned int rangedTexture = 0;
    unsigned int defenseTexture = 0;
    bool initialized = false;
};
