#include "Game.h"
#include "utils/WorldLayout.h"
#include "utils/Constants.h"
#include "entities/MeleePlayer.h"
#include "entities/RangedPlayer.h"
#include "stb_image.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>

static const char* spriteVertexShader = R"(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec2 aTex;
    out vec2 TexCoord;
    uniform vec4 spriteRect;
    void main() {
        vec2 position = spriteRect.xy + aPos * spriteRect.zw;
        gl_Position = vec4(position * 2.0 - 1.0, 0.0, 1.0);
        TexCoord = aTex;
    }
)";
static const char* spriteFragmentShader = R"(
    #version 330 core
    in vec2 TexCoord;
    out vec4 FragColor;
    uniform sampler2D image;
    uniform vec4 spriteTint;
    void main() { FragColor = texture(image, TexCoord) * spriteTint; }
)";

Game::Game() : combatSystem(&gameManager, &collisionSystem) {
    gameManager.setGrid(&grid);
}
Game::~Game() { cleanup(); }

GLuint Game::loadTexture(const char* path) {
    int width = 0, height = 0, channels = 0;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &channels, 4);
    if (!data) {
        std::cerr << "Failed to load texture: " << path << '\n';
        return 0;
    }
    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
    textures.push_back(texture);
    return texture;
}
bool Game::init() {
    if (initialized) return true;
    if (!shader.init(spriteVertexShader, spriteFragmentShader)) return false;
    rendere = std::make_unique<Renderer>(shader, 1000.0f, 700.0f);
    backgroundTexture = loadTexture("assets/backgrounds/example2.png");
    defenseTexture = loadTexture("assets/sprites/Defense.png");
    const char* playerNames[] = {"Melee", "Ranged"};
    const char* enemyNames[] = {"EASYENEMY", "MEDIUMENEMY", "HARDENEMY"};
    bool loaded = backgroundTexture && defenseTexture;
    for (int type = 0; type < 2; ++type) {
        for (int frame = 0; frame < 4; ++frame) {
            std::string path = "assets/sprites/" + std::string(playerNames[type]);
            playerMoveTextures[type][frame] = loadTexture((path + "_M" + std::to_string(frame + 1) + ".png").c_str());
            playerAttackTextures[type][frame] = loadTexture((path + "_A" + std::to_string(frame + 1) + ".png").c_str());
            loaded = loaded && playerMoveTextures[type][frame] && playerAttackTextures[type][frame];
        }
    }
    for (int type = 0; type < 3; ++type) {
        for (int frame = 0; frame < 4; ++frame) {
            std::string path = "assets/sprites/enemies/" + std::string(enemyNames[type]) + "/" + enemyNames[type];
            enemyMoveTextures[type][frame] = loadTexture((path + "_M" + std::to_string(frame + 1) + ".png").c_str());
            enemyAttackTextures[type][frame] = loadTexture((path + "_A" + std::to_string(frame + 1) + ".png").c_str());
            loaded = loaded && enemyMoveTextures[type][frame] && enemyAttackTextures[type][frame];
        }
    }
    meleeTexture = playerMoveTextures[0][0];
    rangedTexture = playerMoveTextures[1][0];
    unsigned char white[] = {255, 255, 255, 255};
    glGenTextures(1, &whiteTexture);
    glBindTexture(GL_TEXTURE_2D, whiteTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    textures.push_back(whiteTexture);
    if (!loaded) { cleanup(); return false; }
    playerCard.init();
    initialized = true;
    startCutscene();
    return true;
}
void Game::cleanup() {
    if (initialized) playerCard.cleanup();
    rendere.reset();
    if (!textures.empty() && glfwGetCurrentContext())
        glDeleteTextures(static_cast<GLsizei>(textures.size()), textures.data());
    textures.clear();
    shader.cleanup();
    initialized = false;
}
Player* Game::placePlayer(PlayerType type, int lane) {
    if (paused || hasWon() || hasLost() || cutscene.isPlaying()) return nullptr;
    return gameManager.placePlayer(type, lane);
}
Player* Game::purchasePlayer(PlayerType type, int lane) {
    int cost = economy.getPlayerCost(type);
    if (!economy.isUnlocked(type) || !economy.canAfford(cost)) return nullptr;
    Player* player = placePlayer(type, lane);
    if (player) economy.spendCoins(cost); // Failed placement never spends coins.
    return player;
}
void Game::reset() {
    combatSystem.setEnemies({});
    gameManager.clear();
    waveManager.reset();
    economy.reset();
    homeBase.reset();
    grid = Grid();
    gameManager.setGrid(&grid);
    cutscene.skip();
    accumulator = 0;
    animationTime = 0;
    paused = false;
}
void Game::jumpSelectedLanes() {
    if (paused || hasWon() || hasLost() || cutscene.isPlaying()) return;
    for (Player* player : gameManager.getActivePlayers()) {
        if (grid.getLanes()[player->laneIndex].selected) player->jump();
    }
}
void Game::duckSelectedLanes() {
    if (paused || hasWon() || hasLost() || cutscene.isPlaying()) return;
    for (Player* player : gameManager.getActivePlayers()) {
        if (grid.getLanes()[player->laneIndex].selected) player->duck();
    }
}
void Game::collectDrops() {
    for (std::size_t i = waveManager.getDropCount(); i > 0; --i) {
        Collectible* drop = waveManager.getDropAt(i - 1);
        for (Player* player : gameManager.getActivePlayers()) {
            if (player->laneIndex == drop->getLane() && collisionSystem.intersects(*player, *drop)) {
                if (drop->collect()) economy.addCoins(drop->getValueOfCoins());
                break;
            }
        }
        if (drop->isCollected()) waveManager.removeDropAt(i - 1);
    }
}
void Game::updateStep(float deltaTime) {
    // Refresh encounters before movement, without retaining pointers across waves.
    std::vector<Enemy*> enemies;
    for (std::size_t i = 0; i < waveManager.getActiveEnemyCount(); ++i)
        enemies.push_back(waveManager.getEnemyAt(i));
    combatSystem.updateEncounters(enemies);
    gameManager.update(deltaTime);
    waveManager.update(deltaTime, true);
    if (waveManager.hasEnemyReachedBase(WorldLayout::HOME_BASE_X)) {
        homeBase.enemyReached();
        return;
    }
    combatSystem.update(deltaTime, waveManager);
    waveManager.finishUpdate();
    combatSystem.setEnemies({});
    collectDrops();
    gameManager.cleanupInactiveEntities();
    animationTime += deltaTime;
}
void Game::update(float deltaTime) {
    if (!std::isfinite(deltaTime) || deltaTime < 0 || paused || hasWon() || hasLost()) return;
    if (cutscene.isPlaying()) { cutscene.update(deltaTime); return; }
    // A fixed small step keeps characters from passing through each other.
    const double step = 1.0 / 120.0;
    accumulator += deltaTime;
    while (accumulator + 0.0000001 >= step) {
        updateStep(static_cast<float>(step));
        accumulator -= step;
        if (hasWon() || hasLost()) { accumulator = 0; break; }
    }
}
void Game::drawHealth(float x, float y, float width, int health, int maxHealth) {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    hud.drawhealth(viewport[0] + static_cast<int>(x * viewport[2]),
                   viewport[1] + static_cast<int>(y * viewport[3]),
                   std::max(12, static_cast<int>(width * viewport[2])), 5, health, maxHealth);
}
void Game::renderPlayers() {
    if (!initialized) return;
    for (Player* player : gameManager.getActivePlayers()) {
        GLuint texture = defenseTexture;
        bool attacking = false;
        if (auto* melee = dynamic_cast<MeleePlayer*>(player)) attacking = melee->isAttacking();
        if (auto* ranged = dynamic_cast<RangedPlayer*>(player)) attacking = ranged->isAttacking();
        if (player->getPlayerType() != PlayerType::Defense) {
            int type = player->getPlayerType() == PlayerType::Melee ? 0 : 1;
            int frame = player->animation.getFrame(4, attacking ? 8.0f : 6.0f);
            texture = attacking ? playerAttackTextures[type][frame] : playerMoveTextures[type][frame];
        }
        float height = player->height;
        float y = WorldLayout::toRenderY(player->y, height);
        if (player->isJumping()) y += .04f * std::sin(3.1415926f * player->jumpTimer / Constants::JUMP_DURATION);
        if (player->isDucking()) height *= .55f;
        float tint = .25f + .75f * player->getHealth() / player->getMaxHealth();
        if (player->damageFlashTimer > 0) tint *= .65f;
        rendere->drawSprite(texture, player->x, y, player->width, height, 1, tint, tint);
        drawHealth(player->x, y + height + .006f, player->width, player->getHealth(), player->getMaxHealth());
    }
}
void Game::renderEnemies() {
    int frame = static_cast<int>(animationTime * 6) % 4;
    for (std::size_t i = 0; i < waveManager.getActiveEnemyCount(); ++i) {
        const Enemy* enemy = waveManager.getEnemyAt(i);
        if (enemy->getState() == EnemyState::DEAD) continue;
        int type = static_cast<int>(enemy->getEnemyType());
        bool attacking = enemy->isMovementBlocked() || enemy->getTimeSinceLastAttack() < Constants::attackDuration;
        GLuint texture = attacking ? enemyAttackTextures[type][frame] : enemyMoveTextures[type][frame];
        float y = WorldLayout::toRenderY(enemy->getPosition().y, WorldLayout::ENEMY_HEIGHT);
        float tint = .25f + .75f * enemy->getHealth() / enemy->getMaxHealth();
        rendere->drawSprite(texture, enemy->getPosition().x, y, WorldLayout::ENEMY_WIDTH,
                             WorldLayout::ENEMY_HEIGHT, 1, tint, tint);
        drawHealth(enemy->getPosition().x, y + WorldLayout::ENEMY_HEIGHT + .006f,
                   WorldLayout::ENEMY_WIDTH, enemy->getHealth(), enemy->getMaxHealth());
    }
}
void Game::renderProjectiles() {
    if (!initialized) return;
    for (Projectile* shot : gameManager.getActiveProjectiles()) {
        float y = WorldLayout::toRenderY(shot->y, shot->height);
        // A bright core and a narrower colored trail make a small energy bolt.
        rendere->drawSprite(whiteTexture, shot->x - shot->width, y + shot->height * .3f,
                             shot->width * 2, shot->height * .4f, .1f, .8f, 1);
        rendere->drawSprite(whiteTexture, shot->x, y, shot->width, shot->height, .7f, 1, 1);
    }
    for (std::size_t i = 0; i < waveManager.getEnemyProjectilesCount(); ++i) {
        EnemyProjectile* shot = waveManager.getEnemyProjectileAt(i);
        if (!shot->isActive()) continue;
        float y = WorldLayout::toRenderY(shot->y, shot->height);
        bool high = shot->getAttackType() == AttackType::High;
        rendere->drawSprite(whiteTexture, shot->x, y, shot->width * 2, shot->height,
                             1, high ? .25f : .75f, .1f);
        rendere->drawSprite(whiteTexture, shot->x, y + shot->height * .25f,
                             shot->width, shot->height * .5f, 1, 1, .8f);
    }
}
void Game::renderCollectibles() {
    for (std::size_t i = 0; i < waveManager.getDropCount(); ++i) {
        const Collectible* drop = waveManager.getDropAt(i);
        if (drop->isCollected()) continue;
        float x = drop->getPosition().x;
        float y = WorldLayout::toRenderY(drop->getPosition().y, WorldLayout::COLLECTIBLE_HEIGHT);
        float w = WorldLayout::COLLECTIBLE_WIDTH, h = WorldLayout::COLLECTIBLE_HEIGHT;
        rendere->drawSprite(whiteTexture, x, y, w, h, .8f, .4f, .05f);
        rendere->drawSprite(whiteTexture, x + w * .15f, y + h * .15f, w * .7f, h * .7f, 1, .8f, .2f);
        rendere->drawSprite(whiteTexture, x + w * .4f, y + h * .25f, w * .15f, h * .5f, 1, 1, .7f);
    }
}
void Game::renderHomeBase() {
    // Keep the detailed reactor in the original background visible. These small
    // lights mark its safe-zone boundary without covering the artwork.
    rendere->drawSprite(whiteTexture, .167f, .08f, .003f, .44f, .3f, 1, .9f, .65f);
    for (int i = 0; i < 3; ++i)
        rendere->drawSprite(whiteTexture, .125f, .15f + i * .11f, .025f, .006f, .3f, 1, .9f, .8f);
}
void Game::renderCutscene() {
    float time = cutscene.getTime();
    int frame = static_cast<int>(time * 6) % 4;
    bool battle = time > 6;
    float jump = time > 9 ? .03f * std::fabs(std::sin((time - 9) * 3)) : 0;
    rendere->drawSprite(battle ? playerAttackTextures[0][frame] : playerMoveTextures[0][frame],
                         cutscene.getPlayerX(), .30f + jump, .10f, .16f);
    rendere->drawSprite(battle ? enemyAttackTextures[0][frame] : enemyMoveTextures[0][frame],
                         cutscene.getEnemyX(), .30f, .10f, .16f);
    if (battle) {
        float x = cutscene.getEnemyX() - std::fmod(time, 1.0f) * .25f;
        rendere->drawSprite(whiteTexture, x, .39f, .018f, .009f, 1, .3f, .1f);
    }
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    hud.drawText(cutscene.getDialogue(), viewport[2] / 12, viewport[3] / 12,
                 std::max(1, viewport[2] / 500));
}
void Game::render(int hoveredLane, int hoveredCard) {
    if (!initialized || !rendere) return;
    rendere->drawSprite(backgroundTexture, 0, 0, 1, 1);
    renderHomeBase();
    if (cutscene.isPlaying()) { renderCutscene(); return; }
    // Keep the original lane-selection and hover colors behind the characters.
    for (const Lane& lane : grid.getLanes()) {
        if (lane.selected)
            rendere->drawSprite(whiteTexture, lane.left, 1.0f - lane.bottom,
                                 lane.right - lane.left, lane.bottom - lane.top, .6f, .2f, 1, .25f);
        else if (lane.id == hoveredLane)
            rendere->drawSprite(whiteTexture, lane.left, 1.0f - lane.bottom,
                                 lane.right - lane.left, lane.bottom - lane.top, 0, .9f, 1, .18f);
    }
    renderCollectibles();
    renderPlayers();
    renderEnemies();
    renderProjectiles();
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    hud.drawCoins(economy.getCoins(), viewport[2], viewport[3]);
    int textScale = std::max(1, viewport[2] / 700);
    std::string waveLabel = "WAVE " + std::to_string(waveManager.getCurrentWave()) + " OF 3";
    hud.drawText(waveLabel.c_str(), viewport[2] * 3 / 4, viewport[3] * 4 / 5, textScale);
    hud.drawText("CITY CORE", viewport[2] / 50, viewport[3] / 25, textScale);
    hud.drawText("SELECT LANES - CLICK A CARD - W JUMP - S DUCK - ESC PAUSE",
                 viewport[2] / 5, viewport[3] / 50, std::max(1, viewport[2] / 1100));
    playerCard.draw(hoveredCard, &economy);
    if (hasWon()) hud.drawText("CITY CORE SAFE - YOU WON", viewport[2] / 4, viewport[3] / 2, 3);
    if (hasLost()) hud.drawText("CITY CORE LOST", viewport[2] / 3, viewport[3] / 2, 3);
}
