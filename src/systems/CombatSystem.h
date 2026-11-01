#include "GameManager.h"
#include "CollisionSystem.h"
#include "../entities/Player.h"
#include "../entities/Enemy.h"
#include "../entities/Projectile.h"
#include <vector>
#include <iostream>
#include <algorithm>

class CombatSystem {
    private :
        GameManager* playerManager;
        CollisionSystem* collisionSystem;
        std::vector<Enemy*> enemies;


    public:
        CombatSystem(GameManager* playerManager, CollisionSystem* collisionSystem);
        void update(float deltaTime, const std::vector<Enemy*>& activeEnemies);
        void setEnemies(const std::vector<Enemy*>& activeEnemies);
        void resolvePlayerAttacks(const std::vector<Enemy*>& activeEnemies);
        void resolveEnemyAttacks(const std::vector<Enemy*>& activeEnemies);
        void updateEncounters(const std::vector<Enemy*>& activeEnemies);
        bool isPlayerProtected(const Player& player, const std::vector<Enemy*>& activeEnemies) const;
        void resolvePlayerAttack(Player& player, Enemy& enemy);
        void resolveEnemyAttack(Enemy& enemy, Player& player);
        void setPlayerManager(GameManager* playerManager);


}

