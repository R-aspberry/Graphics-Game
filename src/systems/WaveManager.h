#pragma once
#include "../entities/Enemy.h"
#include "../entities/Collectible.h"
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
        std::vector<bool> dropGenerated;
        std::vector<Collectible> collectibles;
        bool spawnEnemy();
        bool isWaveComplete() const;
        void startNextWave() ;
        void processEnemyDeaths();
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
        std::size_t getDropCount() const {
            return collectibles.size();
        }
        const Collectible* getDropAt(std::size_t index) const {
            if (index >= collectibles.size()) {
                return nullptr;
            }

            return &collectibles[index];
        }
        Collectible* getDropAt(std::size_t index) {
            if (index >= collectibles.size()) {
                return nullptr;
            }

            return &collectibles[index];
        }
        bool removeDropAt(std::size_t index);
        bool hasEnemyReachedBase(float homeBaseX) const;
};