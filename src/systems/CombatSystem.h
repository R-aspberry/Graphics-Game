#pragma once
#include <vector>

class GameManager;
class CollisionSystem;
class Enemy;
class Player;
class DefensePlayer;
class EnemyProjectile;
class WaveManager;

class CombatSystem {
public:
    CombatSystem(GameManager* playerManager, CollisionSystem* collisionSystem);
    // Call once after managers advance movement and cooldowns. Combat does not
    // advance either. Use the WaveManager overload to include enemy projectiles.
    void update(float deltaTime, const std::vector<Enemy*>& activeEnemies);
    void update(float deltaTime, const std::vector<Enemy*>& activeEnemies,
                const std::vector<EnemyProjectile*>& enemyProjectiles);
    void update(float deltaTime, WaveManager& waveManager);
    void setPlayerManager(GameManager* playerManager);
    // Legacy non-owning snapshot; refresh after wave transitions.
    void setEnemies(const std::vector<Enemy*>& activeEnemies);
    void updateEncounters(const std::vector<Enemy*>& activeEnemies);
    void resolvePlayerAttacks(const std::vector<Enemy*>& activeEnemies);
    void resolveEnemyAttacks(const std::vector<Enemy*>& activeEnemies);
    void resolvePlayerAttack(Player& player, Enemy& enemy);
    void resolveEnemyAttack(Enemy& enemy, Player& player);
    bool isPlayerProtected(const Player& player, const std::vector<Enemy*>& activeEnemies) const;
    void resolvePlayerProjectiles(float deltaTime, const std::vector<Enemy*>& activeEnemies);
    void resolveEnemyProjectiles(float deltaTime, const std::vector<EnemyProjectile*>& projectiles);
private:
    GameManager* playerManager;
    CollisionSystem* collisionSystem;
    std::vector<Enemy*> enemies;
    bool inMeleeRange(const Player& player, const Enemy& enemy, float range = 0.0f) const;
    DefensePlayer* findProtector(const Player& player) const;
};
