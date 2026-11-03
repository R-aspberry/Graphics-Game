#ifndef GAME_H
#define GAME_H

#include "Shader.h"
#include "Renderer.h"
#include "systems/GameManager.h"
#include "systems/CollisionSystem.h"
#include "systems/CombatSystem.h"
#include <vector>
#include <memory>
#include <array>
#include "world/Lane.h"
#include "entities/Enemy.h"


class Game

{
private :
    GameManager gameManager;
    CollisionSystem collisionSystem;
    CombatSystem combatSystem;
    Shader shader;
    std::unique_ptr<Renderer> rendere;
    GLuint meleeTexture = 0;
    GLuint rangedTexture = 0;
    GLuint defenseTexture = 0;
    std::array<std::array<GLuint, 4>, 3> enemyMoveTextures{};
    
    bool initialized ;

public :
    void render(const Lane& lane, PlayerType type);
    void renderEnemy(const Enemy& enemy, const Lane& lane, float animationTime);
    Game();
    ~Game();
    bool init();
    void setGrid(Grid* grid);
    void update(float deltaTime);
    void update(float deltaTime, const std::vector<Enemy*>& enemies);
    void jumpPlayerInLane(int laneIndex);
    void duckPlayerInLane(int laneIndex);
    void reset();
    void render();
    void renderPlayers();
    void renderProjectiles();
    Player* placePlayer(PlayerType player, int lane);
    GLuint loadTexture(const char* path);

};

#endif