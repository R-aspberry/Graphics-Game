#include "Game.h"
#include "utils/ResourceManager.h"
#include "world/Lane.h"

#include <iostream>

// Vertex shader: positions and sizes the sprite.
const char* spriteVertexShader = R"(
    #version 330 core

    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec2 aTex;

    out vec2 TexCoord;
    uniform vec4 spriteRect;

    void main()
    {
        vec2 position = spriteRect.xy + aPos * spriteRect.zw;
        gl_Position = vec4(position * 2.0 - 1.0, 0.0, 1.0);
        TexCoord = aTex;
    }
)";

// Fragment shader: samples the PNG, including its alpha channel.
const char* spriteFragmentShader = R"(
    #version 330 core

    in vec2 TexCoord;
    out vec4 FragColor;

    uniform sampler2D image;

    void main()
    {
        FragColor = texture(image, TexCoord);
    }
)";

Game::~Game()
{
    delete renderer;
    delete spriteShader;

    // Texture cleanup should happen while the OpenGL context is alive.
    // ResourceManager::clear() can be called from main before destroying
    // the window once gameplay textures are no longer needed.
}

void Game::init()
{
    if (initialized)
    {
        return;
    }

    spriteShader = new Shader(
        spriteVertexShader,
        spriteFragmentShader
    );

    if (spriteShader->ID == 0)
    {
        std::cerr << "Failed to initialize sprite shader.\n";
        return;
    }

    renderer = new Renderer(*spriteShader);

    meleeTexture = ResourceManager::loadTexture(
        "assets/sprites/Melee_M1.png"
    );
    
    rangedTexture = ResourceManager::loadTexture(
        "assets/sprites/Ranged_M1.png"
    );

    defenseTexture = ResourceManager::loadTexture(
        "assets/sprites/Defense.png"
    );

    if (meleeTexture == 0)
    {
        std::cerr << "Failed to load Melee sprite.\n";
        return;
    }

    initialized = true;
    std::cout << "Gameplay renderer initialized.\n";
}

void Game::update(float deltaTime)
{
    // We'll add animation and enemy movement here next.
    (void)deltaTime;
}




void Game::render(const Lane& lane, player type)
{
    if (!initialized || renderer == nullptr)
    {
        return;
    }

    // MEDIUM-SIZED PLAYER
    float playerWidth = 0.05f;
    float playerHeight = 0.10f;

    // Position player on the LEFT of its lane
    float playerX = lane.left + 0.025f;

    // Find the vertical CENTER of the lane
    float laneCenterY = (lane.top + lane.bottom) / 2.0f;

    // Center the entire player vertically
    float playerY = 1.0f - laneCenterY - playerHeight / 2.0f;

if (type == player::MELEE) {
    renderer->drawSprite(
        meleeTexture,
        playerX,
        playerY,
        playerWidth,
        playerHeight
    );
    
}
if (type == player::RANGED) {
    renderer->drawSprite(
        rangedTexture,
        playerX,
        playerY,
        playerWidth,
        playerHeight
    );
}
if (type == player::DEFENSE) {
    renderer->drawSprite(
        defenseTexture,
        playerX,
        playerY,
        playerWidth,
        playerHeight
    );
}
}