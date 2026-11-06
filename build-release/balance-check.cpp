#include "systems/GameManager.h"
#include "systems/CombatSystem.h"
#include "systems/WaveManager.h"
#include "systems/Economy.h"
#include "entities/EasyEnemy.h"
#include "entities/MediumEnemy.h"
#include "entities/HardEnemy.h"
#include "utils/Constants.h"
#include <iomanip>
#include <iostream>
#include <limits>

// Diagnostic only: exercise production systems without rendering or UI input.
struct World {
    Grid grid;
    GameManager manager;
    CollisionSystem collision;
    CombatSystem combat{&manager, &collision};
    WaveManager waves;
    Economy economy;
    int earned = 0;
    int purchases = 0;
    World() { manager.setGrid(&grid); }
    void dodge(const std::vector<EnemyProjectile*>& shots) {
        for (auto* p : manager.getActivePlayers()) {
            EnemyProjectile* nearest = nullptr;
            for (auto* s : shots) {
                float distance = s->getX() - (p->getX() + p->getWidth());
                if (s->isActive() && s->getLaneIndex() == p->getLaneIndex() &&
                    distance >= -0.015f && distance < 0.035f &&
                    (!nearest || s->getX() < nearest->getX())) nearest = s;
            }
            if (nearest && !p->isJumping() && !p->isDucking()) {
                if (nearest->getAttackType() == AttackType::High) p->duck();
                else p->jump();
            }
        }
    }
    void collect() {
        for (std::size_t i = waves.getDropCount(); i > 0; --i) {
            auto* drop = waves.getDropAt(i - 1);
            for (auto* p : manager.getActivePlayers()) {
                if (collision.playerTouchesCollectible(*p, *drop) && drop->collect()) {
                    int reward = drop->getValueOfCoins();
                    economy.addCoins(reward);
                    earned += reward;
                    waves.removeDropAt(i - 1);
                    break;
                }
            }
        }
    }
    void buy(bool mixed) {
        Enemy* threat = nullptr;
        for (std::size_t i = 0; i < waves.getActiveEnemyCount(); ++i) {
            auto* e = waves.getEnemyAt(i);
            if (e->getHealth() <= 0) continue;
            bool covered = false;
            for (auto* p : manager.getActivePlayers()) {
                if (p->getLaneIndex() == e->getLane() &&
                    p->getX() < e->getPosition().x + WorldLayout::ENEMY_WIDTH)
                    covered = true;
            }
            if (!covered && (!threat || e->getPosition().x < threat->getPosition().x))
                threat = e;
        }
        if (!threat) return;
        bool ranged = mixed && earned >= 20 && economy.canAfford(20);
        int cost = ranged ? 20 : 10;
        if (economy.canAfford(cost) && manager.placePlayer(
                ranged ? PlayerType::Ranged : PlayerType::Melee, threat->getLane())) {
            economy.spendCoins(cost);
            ++purchases;
        }
    }
};

void duel(PlayerType playerType, EnemyType enemyType, float dt, bool dodge) {
    World w;
    auto* p = w.manager.placePlayer(playerType, 0);
    const auto& lane = w.grid.getLanes()[0];
    glm::vec2 spawn(WorldLayout::enemySpawnX(lane),
                    WorldLayout::standingY(lane, WorldLayout::ENEMY_HEIGHT));
    std::unique_ptr<Enemy> e;
    if (enemyType == EnemyType::EASY) e = std::make_unique<EasyEnemy>(0, spawn, GameConfig::EASY_ENEMY_ATTACK_INTERVAL);
    else if (enemyType == EnemyType::MEDIUM) e = std::make_unique<MediumEnemy>(0, spawn, GameConfig::MEDIUM_ENEMY_ATTACK_INTERVAL);
    else e = std::make_unique<HardEnemy>(0, spawn, GameConfig::HARD_ENEMY_ATTACK_INTERVAL);
    float t = 0;
    while (t < 90 && p->isAlive() && e->getHealth() > 0 &&
           e->getPosition().x > WorldLayout::HOME_BASE_X) {
        e->update(dt);
        w.waves.spawnEnemyProjectile(e.get());
        std::vector<EnemyProjectile*> shots;
        for (std::size_t i = 0; i < w.waves.getEnemyProjectilesCount(); ++i) {
            auto* s = w.waves.getEnemyProjectileAt(i);
            s->update(dt);
            shots.push_back(s);
        }
        if (dodge) w.dodge(shots);
        w.combat.resolveEnemyProjectiles(shots);
        w.manager.update(dt);
        w.combat.update(dt, {e.get()});
        w.manager.removeInactiveProjectiles();
        t += dt;
    }
    std::cout << "duel player=" << static_cast<int>(playerType)
              << " enemy=" << static_cast<int>(enemyType) << " fps=" << int(1/dt + .5f)
              << " dodge=" << dodge << " seconds=" << t
              << " playerHP=" << p->getHealth() << " enemyHP=" << e->getHealth() << '\n';
}

void sessions(bool mixed, bool dodge, int trials) {
    int wins = 0, losses = 0, timeouts = 0;
    int reached[4] = {}, buys = 0;
    double duration = 0;
    constexpr float dt = 1.0f / 60.0f;
    for (int trial = 0; trial < trials; ++trial) {
        World w;
        float t = 0;
        bool lost = false;
        while (t < 600 && !w.waves.hasWon() && !lost) {
            w.buy(mixed);
            w.waves.update(dt);
            std::vector<EnemyProjectile*> shots;
            for (std::size_t i = 0; i < w.waves.getEnemyProjectilesCount(); ++i)
                shots.push_back(w.waves.getEnemyProjectileAt(i));
            if (dodge) w.dodge(shots);
            w.combat.resolveEnemyProjectiles(shots);
            std::vector<Enemy*> enemies;
            for (std::size_t i = 0; i < w.waves.getActiveEnemyCount(); ++i)
                enemies.push_back(w.waves.getEnemyAt(i));
            w.manager.update(dt);
            w.combat.update(dt, enemies);
            w.collect();
            lost = w.waves.hasEnemyReachedBase(WorldLayout::HOME_BASE_X);
            w.manager.cleanupInactiveEntities();
            t += dt;
        }
        if (lost) ++losses;
        else if (w.waves.hasWon()) ++wins;
        else {
            ++timeouts;
            if (timeouts <= 2) {
                std::cout << "timeout wave=" << w.waves.getCurrentWave()
                          << " spawned=" << w.waves.getEnemiesSpawnedInWave() << '\n';
                for (std::size_t i = 0; i < w.waves.getActiveEnemyCount(); ++i) {
                    auto* e = w.waves.getEnemyAt(i);
                    if (e->getHealth() > 0)
                        std::cout << " alive enemy type=" << int(e->getEnemyType())
                                  << " lane=" << e->getLane() << " x=" << e->getPosition().x
                                  << " hp=" << e->getHealth() << " blocked=" << e->isMovementBlocked() << '\n';
                }
                for (auto* p : w.manager.getActivePlayers())
                    std::cout << " alive player type=" << int(p->getPlayerType())
                              << " lane=" << p->getLaneIndex() << " x=" << p->getX()
                              << " hp=" << p->getHealth() << " state=" << int(p->getActionState()) << '\n';
            }
        }
        ++reached[w.waves.getCurrentWave()];
        buys += w.purchases;
        duration += t;
    }
    std::cout << "sessions mixed=" << mixed << " dodge=" << dodge
              << " trials=" << trials << " wins=" << wins << " losses=" << losses
              << " timeouts=" << timeouts << " finalWaves=" << reached[1] << ',' << reached[2] << ',' << reached[3]
              << " avgSeconds=" << duration/trials << " avgPurchases=" << double(buys)/trials << '\n';
}

int main(int argc, char**) {
    std::cout << std::fixed << std::setprecision(2);
    if (argc > 1) { sessions(true, true, 100); return 0; }
    Grid grid;
    for (const auto& lane : grid.getLanes()) {
        float distance = WorldLayout::enemySpawnX(lane) - WorldLayout::HOME_BASE_X;
        std::cout << "lane=" << lane.id << " enemyToBaseNormal=" << distance / GameConfig::EASY_ENEMY_SPEED
                  << " enemyToBaseHard=" << distance / GameConfig::HARD_ENEMY_SPEED << '\n';
    }
    for (float dt : {1.0f/30, 1.0f/60, 1.0f/144})
        for (auto player : {PlayerType::Melee, PlayerType::Ranged, PlayerType::Defense})
            for (auto enemy : {EnemyType::EASY, EnemyType::MEDIUM, EnemyType::HARD})
                duel(player, enemy, dt, false);
    sessions(false, false, 100);
    sessions(true, false, 100);
    sessions(false, true, 100);
    sessions(true, true, 100);
}
