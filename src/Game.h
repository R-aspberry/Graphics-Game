#ifndef GAME_H
#define GAME_H

#include "Shader.h"
#include "Renderer.h"
#include "systems/GameManager.h"
#include "systems/WaveManager.h"
#include "systems/CollisionSystem.h"
#include "systems/CombatSystem.h"
#include "systems/Economy.h"
#include "world/HomeBase.h"
#include "cutscene/Cutscene.h"
#include "ui/HUD.h"
#include "ui/PlayerCard.h"
#include <memory>
#include <vector>

class Game {
private:
    Grid grid;
    GameManager gameManager;
    WaveManager waveManager;
    CollisionSystem collisionSystem;
    CombatSystem combatSystem;
    Economy economy;
    HomeBase homeBase;
    Cutscene cutscene;
    HUD hud;
    PlayerCard playerCard;
    Shader shader;
    std::unique_ptr<Renderer> rendere;
    GLuint meleeTexture = 0, rangedTexture = 0, defenseTexture = 0;
    GLuint backgroundTexture = 0, whiteTexture = 0;
    GLuint playerMoveTextures[2][4] = {};
    GLuint playerAttackTextures[2][4] = {};
    GLuint enemyMoveTextures[3][4] = {};
    GLuint enemyAttackTextures[3][4] = {};
    std::vector<GLuint> textures;
    bool initialized = false;
    bool paused = false;
    double accumulator = 0;
    float animationTime = 0;

    void updateStep(float deltaTime);
    void renderEnemies();
    void renderCollectibles();
    void renderHomeBase();
    void renderCutscene();
    void drawHealth(float x, float y, float width, int health, int maxHealth);
public:
    Game();
    ~Game();
    bool init();
    void cleanup(); // Call before destroying the OpenGL context.
    void update(float deltaTime);
    void render(int hoveredLane = -1, int hoveredCard = -1);
    void renderPlayers();
    void renderProjectiles();
    Player* placePlayer(PlayerType type, int lane);
    Player* purchasePlayer(PlayerType type, int lane);
    GLuint loadTexture(const char* path);
    void collectDrops();
    void reset();
    void jumpSelectedLanes();
    void duckSelectedLanes();
    void setPaused(bool value) { paused = value; }
    bool isPaused() const { return paused; }
    bool hasWon() const { return waveManager.hasWon(); }
    bool hasLost() const { return homeBase.isDestroyed(); }
    void startCutscene() { cutscene.start(); accumulator = 0; }
    void skipCutscene() { cutscene.skip(); }
    bool isCutscenePlaying() const { return cutscene.isPlaying(); }
    Grid& getGrid() { return grid; }
    GameManager& getGameManager() { return gameManager; }
    WaveManager& getWaveManager() { return waveManager; }
    Economy& getEconomy() { return economy; }
};
#endif
