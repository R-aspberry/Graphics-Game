#include "Game.h"
#include "utils/ResourceManager.h"
#include "world/Lane.h"

#include <iostream>
#include "entities/Enemy.h"
#include "entities/EasyEnemy.h"
#include "entities/MediumEnemy.h"
#include "entities/HardEnemy.h"
#include <string>
#include <memory>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const char *spriteVertexShader = R"(
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

const char *spriteFragmentShader = R"(
    #version 330 core

    in vec2 TexCoord;
    out vec4 FragColor;

    uniform sampler2D image;

    void main()
    {
        FragColor = texture(image, TexCoord);
    }
)";

Game::Game() : shader (spriteVertexShader, spriteFragmentShader), initialized(false){

}

Game::~Game() = default;


GLuint Game::loadTexture(const char* path){
    int width, height, channels;
    unsigned char* data = stbi_load(path, &width,&height,&channels,4);
    // if (data == nullptr){
    //     return 0;
    // }


    GLuint textureID =0 ;
    glGenTextures(1,&textureID);
    glBindTexture( GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,data);
    stbi_image_free(data);
    return textureID;

}

bool Game::init(){
    if (shader.Program == 0){
        return false;
    }

    rendere = std::make_unique<Renderer>(shader, 1000.0f, 700.0f);

    meleeTexture = loadTexture("assets/sprites/Melee_M1.png");
    defenseTexture = loadTexture("assets/sprites/Defense.png");
    rangedTexture = loadTexture("assets/sprites/Ranged_M1.png");
    
    if(meleeTexture == 0 || rangedTexture ==0 || defenseTexture == 0){
        return false;
    }

    initialized = true;
    return true;

}

void Game::renderPlayers()
{
    const auto& players = gameManager.getActivePlayers();

    for(const auto& player : players){
        GLuint texture =0;

        if(player->getPlayerType() == PlayerType::Defense){
            texture = defenseTexture;
        }
        if(player->getPlayerType() == PlayerType::Melee){
            texture = meleeTexture;
        }
        if(player->getPlayerType() == PlayerType::Ranged){
            texture = rangedTexture;
        }

        rendere->drawSprite(texture, player->getX(), player->getY(), player->getWidth(), player->getHeight());
    }

}

void Game::render() {

    if (!initialized || rendere == nullptr) {
        return;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    renderPlayers();
}

void Game::update(float deltaTime){
    gameManager.update(deltaTime);
    waveManager.update(deltaTime);
}
// bool Game::init()
// {
//     if (initialized)
//     {
//         return true;
//     }

//     spriteShader = std::make_unique<Shader>(
//         spriteVertexShader,
//         spriteFragmentShader);

//     if (spriteShader->Program == 0)
//     {
//         std::cerr << "Failed to initialize sprite shader.\n";
//         spriteShader.reset();
//         return false;
//     }

//     renderer = std::make_unique<Renderer>(*spriteShader, 1000.0f, 700.0f);

//     meleeTexture = ResourceManager::loadTexture(
//         "assets/sprites/Melee_M1.png");

//     rangedTexture = ResourceManager::loadTexture(
//         "assets/sprites/Ranged_M1.png");

//     defenseTexture = ResourceManager::loadTexture(
//         "assets/sprites/Defense.png");

//     if (meleeTexture == 0)
//     {
//         std::cerr << "Failed to load Melee sprite.\n";
//         renderer.reset();
//         spriteShader.reset();
//         return false;
//     }

//     const char *enemyFolders[3] = {
//         "EASYENEMY",
//         "MEDIUMENEMY",
//         "HARDENEMY"};

//     for (int type = 0; type < 3; type++)
//     {
//         for (int frame = 0; frame < 4; frame++)
//         {
//             std::string folder = enemyFolders[type];

//             std::string path =
//                 "assets/sprites/enemies/" + folder + "/" +
//                 folder + "_M" + std::to_string(frame + 1) + ".png";

//             enemyMoveTextures[type][frame] =
//                 ResourceManager::loadTexture(path);
//         }
//     }

//     initialized = true;
//     std::cout << "Gameplay renderer initialized.\n";
//     return true;
// }

// void Game::update(float deltaTime)
// {
//     // We'll add animation and enemy movement here next.
//     (void)deltaTime;
// }

// void Game::render(const Lane &lane, Game::player type)
// {
//     if (!initialized || !renderer)
//     {
//         return;
//     }

//     // MEDIUM-SIZED PLAYER
//     float playerWidth = 0.05f;
//     float playerHeight = 0.10f;

//     // Position player on the LEFT of its lane
//     float playerX = lane.left + 0.025f;

//     // Find the vertical CENTER of the lane
//     float laneCenterY = (lane.top + lane.bottom) / 2.0f;

//     // Center the entire player vertically
//     float playerY = 1.0f - laneCenterY - playerHeight / 2.0f;

//     if (type == Game::player::MELEE)
//     {
//         renderer->drawSprite(
//             meleeTexture,
//             playerX,
//             playerY,
//             playerWidth,
//             playerHeight);
//     }
//     if (type == Game::player::RANGED)
//     {
//         renderer->drawSprite(
//             rangedTexture,
//             playerX,
//             playerY,
//             playerWidth,
//             playerHeight);
//     }
//     if (type == Game::player::DEFENSE)
//     {
//         renderer->drawSprite(
//             defenseTexture,
//             playerX,
//             playerY,
//             playerWidth,
//             playerHeight);
//     }
// }

// void Game::renderEnemy(
//     const Enemy &enemy,
//     const Lane &lane,
//     float animationTime)
// {
//     if (!initialized || !renderer)
//     {
//         return;
//     }

//     if (enemy.getState() == EnemyState::DEAD)
//     {
//         return;
//     }

//     int type = 0;

//     if (dynamic_cast<const MediumEnemy *>(&enemy))
//     {
//         type = 1;
//     }
//     else if (dynamic_cast<const HardEnemy *>(&enemy))
//     {
//         type = 2;
//     }

//     int frame = static_cast<int>(animationTime * 6.0f) % 4;

//     unsigned int texture = enemyMoveTextures[type][frame];

//     if (texture == 0)
//     {
//         return;
//     }

//     float width = 0.075f;
//     float height = 0.11f;

//     float x = enemy.getPosition().x / 1000.0f - width / 2.0f;

//     float laneCenterY = (lane.top + lane.bottom) / 2.0f;
//     float y = 1.0f - laneCenterY - height / 2.0f;

//     renderer->drawSprite(texture, x, y, width, height);
// }
