
#include "Game.h"
#include "utils/ResourceManager.h"

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

void Game::render()
{
    if (!initialized || renderer == nullptr)
    {
        return;
    }

    // Draw Melee near the bottom-left of the screen.
    renderer->drawSprite(
        meleeTexture,
        0.08f,  // x
        0.08f,  // y
        0.22f,  // width
        0.35f   // height
    );
}
