# include "WaveManager.h"
#include "../world/Grid.h"
#include "../utils/Constants.h" 
#include "../entities/EasyEnemy.h"
#include "../entities/MediumEnemy.h"
#include "../entities/HardEnemy.h"
#include "../world/Grid.h"
#include <random>
#include <utility>

WaveManager::WaveManager() : currentWave(1), enemiesSpawnedInWave(0), 
                totalEnemiesInWave(GameConfig::WAVE_1_COUNT), spawnTimer(0.0f),gameWon(false) {}


void WaveManager::processEnemyDeaths() {
    for (std::size_t i = 0; i < activeEnemies.size(); ++i) {
        if(activeEnemies[i]->getState() == EnemyState::DEAD && !dropGenerated[i]){
            int reward = GameConfig::EASY_ENEMY_REWARD;  
            if(activeEnemies[i]->getEnemyType() == EnemyType::EASY){
                reward = GameConfig::EASY_ENEMY_REWARD;
            }
            else if(activeEnemies[i]->getEnemyType() == EnemyType::MEDIUM){
                reward = GameConfig::MEDIUM_ENEMY_REWARD;
            }
            else if(activeEnemies[i]->getEnemyType() == EnemyType::HARD){
                reward = GameConfig::HARD_ENEMY_REWARD;
            }
            glm::vec2 enemyPosition = activeEnemies[i]->getPosition();

            glm::vec2 dropPosition(
                enemyPosition.x + (WorldLayout::ENEMY_WIDTH - WorldLayout::COLLECTIBLE_WIDTH) / 2.0f,

                enemyPosition.y + (WorldLayout::ENEMY_HEIGHT - WorldLayout::COLLECTIBLE_HEIGHT) / 2.0f
            );

            collectibles.push_back({
                activeEnemies[i]->getLane(),
                dropPosition,
                reward
            });
            dropGenerated[i] = true;
        }
    }
}


void WaveManager::startNextWave() {
    if (currentWave >= 3 || !isWaveComplete()) {
        return;
    }

    currentWave++;

    enemiesSpawnedInWave = 0;
    spawnTimer = 0.0f;

    if (currentWave == 2) {
        totalEnemiesInWave = GameConfig::WAVE_2_COUNT;
    }
    else if (currentWave == 3) {
        totalEnemiesInWave = GameConfig::WAVE_3_COUNT;
    }

    activeEnemies.clear();
    dropGenerated.clear();
    enemyProjectiles.clear();
}


bool WaveManager::isWaveComplete() const {
    if(enemiesSpawnedInWave < totalEnemiesInWave) {
        return false;
    }
    for (const auto& enemy : activeEnemies) {
        if (enemy->getState() != EnemyState::DEAD) {
            return false;
        }
    }
    return true;
}

Enemy* WaveManager::getEnemyAt(std::size_t index) {
    if (index >= activeEnemies.size()) {
        return nullptr;
    }
    return activeEnemies[index].get();
}

bool WaveManager::spawnEnemy() {
    static std::mt19937 generator{std::random_device{}()}; //random number generator

    std::uniform_int_distribution<int> laneDistribution(
        0, GameConfig::LANE_COUNT - 1
    ); //random lane selection 

    int lane = laneDistribution(generator); //select a random lane

     
    std::unique_ptr<Enemy> enemy;
    EnemyType enemyType = EnemyType::EASY;

   if (currentWave == 2) {
        if (enemiesSpawnedInWave == 0) {
            enemyType = EnemyType::EASY;
        }
        else if (enemiesSpawnedInWave == 1) {
            enemyType = EnemyType::MEDIUM;
        }
        else {
            std::uniform_int_distribution<int> enemyDistribution(0, 1);
            enemyType = static_cast<EnemyType>(enemyDistribution(generator));
        }
    }
    if (currentWave == 3){
        if (enemiesSpawnedInWave == 0) {
            enemyType = EnemyType::EASY;
        }
        else if (enemiesSpawnedInWave == 1) {
            enemyType = EnemyType::MEDIUM;
        }
        else if (enemiesSpawnedInWave == 2) {
            enemyType = EnemyType::HARD;
        }
        else {
            std::uniform_int_distribution<int> enemyDistribution(0, 2);
            enemyType = static_cast<EnemyType>(enemyDistribution(generator));
        }
    }

    static const Grid grid;
    const auto& lanes = grid.getLanes();

    const Lane& selectedLane = lanes[lane];

    glm::vec2 spawnPosition(
        WorldLayout::enemySpawnX(selectedLane),
        WorldLayout::standingY(selectedLane, WorldLayout::ENEMY_HEIGHT)
    );
    
    if (enemyType == EnemyType::EASY) {
    enemy = std::make_unique<EasyEnemy>(lane, spawnPosition, GameConfig::EASY_ENEMY_ATTACK_INTERVAL);
    }
    else if (enemyType == EnemyType::MEDIUM){
        enemy = std::make_unique<MediumEnemy>(lane, spawnPosition, GameConfig::MEDIUM_ENEMY_ATTACK_INTERVAL);
    }
    else {
        enemy = std::make_unique<HardEnemy>(lane, spawnPosition, GameConfig::HARD_ENEMY_ATTACK_INTERVAL);
    }

    activeEnemies.push_back(std::move(enemy));
    dropGenerated.push_back(false);
    return true;
  
}

bool WaveManager::removeDropAt(std::size_t index) {
    if (index >= collectibles.size()) {
        return false;
    }
    else if (!collectibles[index].isCollected()) {
        return false;
    }
    collectibles.erase(collectibles.begin() + index);
    return true;
}

bool WaveManager::hasEnemyReachedBase(float homeBaseX) const {

    for (const auto& enemy : activeEnemies) {
        if (enemy->getPosition().x <= homeBaseX && enemy->getState() != EnemyState::DEAD) {
            return true;
        }
    }
    return false;
}

bool WaveManager::spawnEnemyProjectile(Enemy *enemy) {
    if (enemy == nullptr) {
        return false;
    }
    if(!enemy->canAttack()){
        return false;
    }
    AttackType type = enemy->getAttackType();
    if(type == AttackType::Melee || type == AttackType::Projectile){
        return false;
    }

    static const Grid grid; 
    const auto& lane = grid.getLanes(); 
    int laneIndex = enemy->getLane();

    if(laneIndex < 0 || laneIndex >= 4){
        return false;
    }
    
    const Lane& enemyLane = lane[laneIndex];
    float playerTopY = WorldLayout::standingY(enemyLane, WorldLayout::PLAYER_HEIGHT);
    float projectileCenterY;

    if (type == AttackType::Low){
        projectileCenterY = playerTopY + 
        WorldLayout::PLAYER_HEIGHT * WorldLayout::LOW_ATTACK_OFFSET;
    }
    else if (type == AttackType::High){
        projectileCenterY = playerTopY +
        WorldLayout::PLAYER_HEIGHT * WorldLayout::HIGH_ATTACK_OFFSET;
    }
    else {
        return false;
    }

    float projectileY = projectileCenterY - EnemyProjectile::HEIGHT / 2.0f;

    glm::vec2 position = enemy->getPosition();
    float projectileX =position.x - EnemyProjectile::WIDTH;

    auto projectile = std::make_unique<EnemyProjectile>(
        projectileX,
        projectileY,
        laneIndex,
        0.10f,
        enemy->getDamage(),
        type
    );
    enemyProjectiles.push_back(std::move(projectile));
    enemy->resetAttackTimer();
    return true;
}
    
void WaveManager::update(float deltaTime) {
    if (deltaTime < 0.0f || gameWon) {
        return;
    }

    // Update existing enemies
    for (auto& enemy : activeEnemies) {
        enemy->update(deltaTime);
        const AttackType type = enemy->getAttackType();
        if (type == AttackType::High || type == AttackType::Low) {
            if (enemy->updateRangedAttack(deltaTime)) {
                spawnEnemyProjectile(enemy.get());
            }
        }
    }
    // Update projectiles
    for(auto& projectile : enemyProjectiles) {
        projectile->update(deltaTime);
        
        if (projectile->getX() + projectile->getWidth() < 0.0f) {
            projectile->setActive(false);
        }
    }
    //delete projectiles that are no longer active
    for(std::size_t i = enemyProjectiles.size(); i > 0; --i) {
        if(!enemyProjectiles[i - 1]->isActive()) {
            enemyProjectiles.erase(enemyProjectiles.begin() + (i - 1));
        }
    }

    // Update spawning timer
    spawnTimer += deltaTime;

    // Spawn enemies
    while (spawnTimer >= GameConfig::ENEMY_SPAWN_INTERVAL &&
           enemiesSpawnedInWave < totalEnemiesInWave) {

        if (!spawnEnemy()) {
            break;
        }

        enemiesSpawnedInWave++;
        spawnTimer -= GameConfig::ENEMY_SPAWN_INTERVAL;
    }
    processEnemyDeaths();
    // Check whether the wave is complete
    if (isWaveComplete()) {
        if (currentWave == 3) {
            gameWon = true;
        }
        else {
            startNextWave();
        }
    }
    
}
