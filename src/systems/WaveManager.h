#pragma once
#include "../entities/Enemy.h"
#include "../entities/Collectible.h"
#include "../entities/EnemyProjectile.h"
#include <memory>
#include  <vector>
#include <cstddef> 
#include <random>
class WaveManager {

    private : 
        int currentWave;
        int enemiesSpawnedInWave;
        int totalEnemiesInWave;
        float spawnTimer;
        std::vector<std::unique_ptr<Enemy>> activeEnemies;
        std::vector<std::unique_ptr<EnemyProjectile>> enemyProjectiles;
        std::vector<bool> dropGenerated;
        std::vector<Collectible> collectibles;
        bool spawnEnemy();
        bool isWaveComplete() const;
        void startNextWave() ;
        void processEnemyDeaths();
        bool gameWon;
        std::mt19937 generator;

    public:
        WaveManager();
        // Game defers cleanup until combat has checked this step's projectiles.
        void update(float deltaTime, bool deferCombat = false);
        void finishUpdate();
        void reset();
        void setRandomSeed(unsigned int seed) { generator.seed(seed); }
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
        std::size_t getEnemyProjectilesCount() const {
            return enemyProjectiles.size();
        }
        EnemyProjectile* getEnemyProjectileAt(std::size_t index) {
            if (index >= enemyProjectiles.size()) {
                return nullptr;
            }
            return enemyProjectiles[index].get();
        }
        bool spawnEnemyProjectile(Enemy *enemy);
        bool removeDropAt(std::size_t index);
        bool hasEnemyReachedBase(float homeBaseX) const;
};
