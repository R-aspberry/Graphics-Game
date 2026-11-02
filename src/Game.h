
#ifndef GAME_H
#define GAME_H

#include "Shader.h"
#include "Renderer.h"
#include "systems/WaveManager.h"
#include "systems/GameManager.h"
#include <vector>
#include <memory>
#include <array>
#include "world/Lane.h"


class Game

{
private :
    GameManager gameManager;
    WaveManager waveManager;
    Shader shader;
    std::unique_ptr<Renderer> rendere;
    GLuint meleeTexture = 0;
    GLuint rangedTexture = 0;
    GLuint defenseTexture = 0;
    
    bool initialized ;

public :
    Game();
    ~Game();
    bool init();
    void update(float deltaTime);
    void render();
    void renderPlayers();
    void renderProjectiles();
    Player* placePlayer(PlayerType player, int lane);
    GLuint loadTexture(const char* path);

};

#endif