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
    const GLuint texture = enemyMoveTextures[3][4];
    float animationTimer = 0.0f;
    GLuint meleeTextureM1 ;
    GLuint meleeTextureM2 ;
    GLuint meleeTextureM3 ;
    GLuint meleeTextureM4 ;
    GLuint meleeTextureA1 ;
    GLuint meleeTextureA2 ;
    GLuint meleeTextureA3 ;
    GLuint meleeTextureA4 ;
    GLuint meleeTextureJ1 ;
    GLuint meleeTextureJ2 ;
    GLuint meleeTextureJ3 ;
    GLuint meleeTextureD1 ;
   GLuint  meleeTextureD2 ;
   GLuint meleeTextureD3 ;



    GLuint defenseTexture ;
    

    GLuint rangedTextureM1 ;
    GLuint rangedTextureM2;
    GLuint rangedTextureM3;
    GLuint rangedTextureM4;
    GLuint rangedTextureA1;
    GLuint rangedTextureA2;
    GLuint rangedTextureA3;
    GLuint rangedTextureA4;

    GLuint rangedTextureD1;
    GLuint rangedTextureD2;
    GLuint rangedTextureD3;

    GLuint rangedTextureJ1;
    GLuint rangedTextureJ2;
    GLuint rangedTextureJ3;

    GLuint projectileTexture = 0;
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
    CombatSystem& getCombatSystem() { return combatSystem; }
    std::vector<Player*> getPlayersInLane(int laneIndex) const {
    return gameManager.getPlayersInLane(laneIndex);
}

};

#endif
