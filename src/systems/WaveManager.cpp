# include "WaveManager.h"
# include "../utils/Constants.h" 
# include "../entities/EasyEnemy.h"
#include "../entities/MediumEnemy.h"
#include "../entities/HardEnemy.h"
# include <random>
# include <utility>

WaveManager::WaveManager() : currentWave(1), enemiesSpawnedInWave(0), 
                totalEnemiesInWave(GameConfig::WAVE_1_COUNT), spawnTimer(0.0f),gameWon(false) {}


void WaveManager::processEnemyDeaths() {
    for (std::size_t i = 0; i < activeEnemies.size(); ++i) {
        if(activeEnemies[i]->getState() == EnemyState::DEAD && !dropGenerated[i]){
            enemyDrops.push_back({
                activeEnemies[i]->getPosition(),
                activeEnemies[i]->getLane()
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

    glm::vec2 spawnPosition(900.0f, 100.0f + lane * 75.0f); //calculate spawn position based on lane
    
       if (enemyType == EnemyType::EASY) {
    enemy = std::make_unique<EasyEnemy>(lane, spawnPosition, 2.0f);
    }
    else if (enemyType == EnemyType::MEDIUM){
        enemy = std::make_unique<MediumEnemy>(lane, spawnPosition, 2.0f);
    }
    else {
        enemy = std::make_unique<HardEnemy>(lane, spawnPosition, 2.0f);
    }

    activeEnemies.push_back(std::move(enemy));
    dropGenerated.push_back(false);
    return true;
  
}

bool WaveManager::removeDropAt(std::size_t index) {
    if (index >= enemyDrops.size()) {
        return false;
    }
    enemyDrops.erase(enemyDrops.begin() + index);
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

void WaveManager::update(float deltaTime) {
    if (deltaTime < 0.0f || gameWon) {
        return;
    }

    // Update existing enemies
    for (auto& enemy : activeEnemies) {
        enemy->update(deltaTime);
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
