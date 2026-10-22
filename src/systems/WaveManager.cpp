# include "WaveManager.h"
# include "../utils/Constants.h" 
# include "../entities/EasyEnemy.h"
#include "../entities/MediumEnemy.h"
#include "../entities/HardEnemy.h"
# include <random>
# include <utility>

WaveManager::WaveManager() : currentWave(1), enemiesSpawnedInWave(0), 
                totalEnemiesInWave(GameConfig::WAVE_1_COUNT), spawnTimer(0.0f) {}

bool WaveManager::spawnEnemy() {
    static std::mt19937 generator{std::random_device{}()}; //random number generator

    std::uniform_int_distribution<int> laneDistribution(
        0, GameConfig::LANE_COUNT - 1
    ); //random lane selection 

    int lane = laneDistribution(generator); //select a random lane

     
    std::unique_ptr<Enemy> enemy;
    EnemyType enemyType = EnemyType::EASY;

    if (currentWave == 2){
        std::uniform_int_distribution<int> enemyDistribution(0, 1);
        enemyType = static_cast<EnemyType>(enemyDistribution(generator));
    }
    if (currentWave == 3){
        std::uniform_int_distribution<int> enemyDistribution(0, 2);
        enemyType = static_cast<EnemyType>(enemyDistribution(generator));
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
    return true;
  
}

void WaveManager::update(float deltaTime) {
    if (deltaTime <0.0f){
        return;
    }
    spawnTimer += deltaTime;
    for(auto& enemy : activeEnemies) {
        enemy->update(deltaTime);
    }
    while(spawnTimer >= GameConfig::ENEMY_SPAWN_INTERVAL && enemiesSpawnedInWave < totalEnemiesInWave) {
        if (!spawnEnemy()) {
            break;
        }
        enemiesSpawnedInWave++;
        spawnTimer -= GameConfig::ENEMY_SPAWN_INTERVAL;
    }

}


