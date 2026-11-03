#include "Game.h"
#include "utils/ResourceManager.h"
#include "world/Lane.h"
#include "utils/WorldLayout.h"
#include "systems/CollisionSystem.h"
#include "systems/WaveManager.h"
#include "entities/Enemy.h"
#include "entities/EasyEnemy.h"
#include "entities/MediumEnemy.h"
#include "entities/HardEnemy.h"
#include "systems/Economy.h"
#include <string>
#include <memory>
#include <algorithm>
#include <iostream>
#include "stb_image.h"

const char *spriteVertexShader = R"(
    #version 330 core

    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec2 aTex;

    out vec2 TexCoord;
    uniform vec4 spriteRect;
    uniform bool enemySprite;
    void main()
    {
        vec2 position = spriteRect.xy + aPos * spriteRect.zw;
        gl_Position = vec4(position * 2.0 - 1.0, 0.0, 1.0);
        TexCoord = aTex;
        if (enemySprite)
        {
            TexCoord.x = 1.0 - TexCoord.x;
        }
    }
)";


const char *spriteFragmentShader = R"(
    #version 330 core

    in vec2 TexCoord;
    out vec4 FragColor;

    uniform sampler2D image;
    uniform bool enemySprite;

    uniform bool solidColor;
    uniform vec3 circleColor;

    void main()
    {
        vec4 pixel = texture(image, TexCoord);

        if (enemySprite && pixel.r < 0.08 &&pixel.g < 0.08 &&pixel.b < 0.08)
            discard;

        FragColor = pixel;

        if (solidColor) {
            FragColor = vec4(circleColor, 1.0);
            return;
        }
    }
)";

Game::Game()
    : combatSystem(&gameManager, &collisionSystem),
      shader(spriteVertexShader, spriteFragmentShader),
      initialized(false) {

}

Game::~Game() = default;


GLuint Game::loadTexture(const char* path){
    int width = 0;
    int height = 0;
    int channels = 0;
    unsigned char* data = stbi_load(path, &width,&height,&channels,4);
    if (data == nullptr) {
        std::cerr << "Failed to load player texture: " << path << '\n';
        return 0;
    }

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
    if (initialized) {
        return true;
    }
    if (shader.Program == 0){
        return false;
    }

    rendere = std::make_unique<Renderer>(shader, 1000.0f, 700.0f);

    meleeTexture = loadTexture("assets/sprites/Melee_M1.png");
    defenseTexture = loadTexture("assets/sprites/Defense.png");
    rangedTexture = loadTexture("assets/sprites/Ranged_M1.png");

    const char* enemyFolders[] = {"EASYENEMY", "MEDIUMENEMY", "HARDENEMY"};
    for (std::size_t type = 0; type < enemyMoveTextures.size(); ++type) {
        for (std::size_t frame = 0; frame < enemyMoveTextures[type].size(); ++frame) {
            const std::string path =
                "assets/sprites/enemies/" + std::string(enemyFolders[type]) +
                "/" + enemyFolders[type] + "_M" + std::to_string(frame + 1) + ".png";
            enemyMoveTextures[type][frame] = ResourceManager::loadTexture(path);
        }
    } // this method should be removed 
    
    if(meleeTexture == 0 || rangedTexture ==0 || defenseTexture == 0){
        return false;
    }

    initialized = true;
    return true;

}

void Game::setGrid(Grid* grid) {
    gameManager.setGrid(grid);
}

void Game::reset() {
    gameManager.clear();
}

Player* Game::placePlayer(PlayerType type, int laneIndex) {
    return gameManager.placePlayer(type, laneIndex);
}

void Game::jumpPlayerInLane(int laneIndex) {
    for (Player* player : gameManager.getPlayersInLane(laneIndex)) {
        if (player->getPlayerType() != PlayerType::Defense) {
            player->jump();
            return;
        }
    }
}

void Game::duckPlayerInLane(int laneIndex) {
    for (Player* player : gameManager.getPlayersInLane(laneIndex)) {
        if (player->getPlayerType() != PlayerType::Defense) {
            player->duck();
            return;
        }
    }
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

        const float width = WorldLayout::PLAYER_WIDTH;
        const float height = WorldLayout::PLAYER_HEIGHT;
        rendere->drawSprite(
            texture,
            player->getX(),
            WorldLayout::toRenderY(player->getY(), height),
            width,
            height);
    }

}

void Game::render(const Lane& lane, PlayerType type) {
    if (!initialized || rendere == nullptr) {
        return;
    }

    GLuint texture = 0;
    switch (type) {
        case PlayerType::Melee:
            texture = meleeTexture;
            break;
        case PlayerType::Ranged:
            texture = rangedTexture;
            break;
        case PlayerType::Defense:
            texture = defenseTexture;
            break;
    }

    if (texture == 0) {
        return;
    }

    const float width = WorldLayout::PLAYER_WIDTH;
    const float height = WorldLayout::PLAYER_HEIGHT;
    const float x = WorldLayout::playerSpawnX(lane);
    const float y = WorldLayout::toRenderY(
        WorldLayout::standingY(lane, height),
        height);
    rendere->drawSprite(texture, x, y, width, height);
} // modified the render method 

void Game::renderEnemy(const Enemy& enemy, const Lane& lane, float animationTime) {
    if (!initialized || rendere == nullptr || enemy.getState() == EnemyState::DEAD) {
        return;
    }

    const auto type = static_cast<std::size_t>(enemy.getEnemyType());
    if (type >= enemyMoveTextures.size()) {
        return;
    }

    const auto frame = static_cast<std::size_t>(animationTime * 6.0f) %
                       enemyMoveTextures[type].size();
    const GLuint texture = enemyMoveTextures[type][frame];
    if (texture == 0) {
        return;
    }

    const float width = WorldLayout::ENEMY_WIDTH;
    const float height = WorldLayout::ENEMY_HEIGHT;
    const float y = WorldLayout::toRenderY(enemy.getPosition().y, height);
    if (enemy.getLane() != lane.id) {
        return;
    }
    shader.use();

        glUniform1i(
            glGetUniformLocation(shader.Program, "enemySprite"), 1
        );

        rendere->drawSprite(texture, enemy.getPosition().x, y, width, height);

        glUniform1i(
            glGetUniformLocation(shader.Program, "enemySprite"), 0
        );
}


void Game::renderCollectibles(const WaveManager& waveManager)
{
    if (!initialized || rendere == nullptr)
        return;

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    if (viewport[2] <= 0 || viewport[3] <= 0)
        return;

    for (std::size_t i = 0; i < waveManager.getDropCount(); i++)
    {
        const Collectible* drop = waveManager.getDropAt(i);

        if (drop == nullptr || drop->isCollected())
            continue;

        float width = drop->getWidthOfCollisionBox();
        float height = drop->getHeightOfCollisionBox();

        // Fit a true circle inside the collectible's box
        float pixelWidth = width * viewport[2];
        float pixelHeight = height * viewport[3];
        float diameter = std::min(pixelWidth, pixelHeight);

        float drawWidth = diameter / viewport[2];
        float drawHeight = diameter / viewport[3];

        // Keep the circle centered in its collision box
        float x = drop->getPosition().x
                + (width - drawWidth) / 2.0f;

        float y = drop->getPosition().y
                + (height - drawHeight) / 2.0f;

        rendere->drawCircle(
            x,
            WorldLayout::toRenderY(y, drawHeight),
            drawWidth,
            drawHeight,
            1.0f, 0.75f, 0.0f
        );
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
    gameManager.cleanupInactiveEntities();
}

void Game::update(float deltaTime, const std::vector<Enemy*>& enemies) {
    gameManager.update(deltaTime);
    combatSystem.update(deltaTime, enemies);
    gameManager.cleanupInactiveEntities();
}

void Game::collectDrops(WaveManager& waveManager, Economy& economy) {
    for(std::size_t i = waveManager.getDropCount(); i> 0 ;) {
        --i;

        Collectible* coin = waveManager.getDropAt(i);

        if (coin == nullptr || coin->isCollected())
            continue;

        for(Player* player : gameManager.getActivePlayers()){
            if(player != nullptr && collisionSystem.playerTouchesCollectible(*player, *coin) && coin->collect()){
                economy.addCoins(coin->getValueOfCoins());
                waveManager.removeDropAt(i);
                break;
            }
        }
    }
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