#pragma once
#include "../entities/Enemy.h"
#include <memory>
#include  <vector>
#include <cstddef>
class WaveManager {

    private : 
        int currentWave;
        int enemiesSpawnedInWave;
        int totalEnemiesInWave;
        float spawnTimer;
        std::vector<std::unique_ptr<Enemy>> activeEnemies;
        bool spawnEnemy();
        bool isWaveComplete() const;
        void startNextWave() ;
        bool gameWon;

    public:
        WaveManager();
        void update(float deltaTime);
        int getEnemiesSpawnedInWave() const {
            return enemiesSpawnedInWave;
        }
        std::size_t getActiveEnemyCount() const{
            return activeEnemies.size();
        }
       const Enemy* getEnemyAt(std::size_t index) const{
        if (index >= activeEnemies.size()) {
            return nullptr;
        }
        return activeEnemies[index].get();
       }
       int getCurrentWave() const{
        return currentWave;
       }
       Enemy* getEnemyAt(std::size_t index);
       bool hasWon() const {
            return gameWon;
        }
};