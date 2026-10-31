
#pragma once

#include "Shader.h"
#include "Renderer.h"
struct Lane;
class Game
{
public:
    Game() = default;
    ~Game();

    void init();
    void update(float deltaTime);
    void render(const Lane& lane);

private:
    Shader* spriteShader = nullptr;
    Renderer* renderer = nullptr;

    unsigned int meleeTexture = 0;
    bool initialized = false;
};
