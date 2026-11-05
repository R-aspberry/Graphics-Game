
#include "../src/systems/CombatSystem.h"
#include "../src/systems/CollisionSystem.h"
#include "../src/systems/GameManager.h"
#include "../src/entities/MeleePlayer.h"
#include "../src/entities/RangedPlayer.h"
#include "../src/entities/DefensePlayer.h"
#include "../src/entities/EasyEnemy.h"
#include "../src/entities/HardEnemy.h"
#include "../src/entities/EnemyProjectile.h"
#include "../src/entities/MediumEnemy.h"
#include "../src/systems/Economy.h"
#include "../src/systems/WaveManager.h"
#include "../src/utils/Constants.h"
#include "../src/world/Grid.h"

#include <iostream>
#include <memory>

int passed = 0;
int failed = 0;

void check(bool condition, const char* name) {
    std::cout << (condition ? "[PASS] " : "[FAIL] ")
              << name << '\n';

    if (condition)
        ++passed;
    else
        ++failed;
}

struct TestGame {
    GameManager manager;
    CollisionSystem collision;
    CombatSystem combat{&manager, &collision};

    template<typename T>
    T* addPlayer(float x = 0.30f, int lane = 0, int hp = 5) {
        auto player = std::make_unique<T>(
            x, 0.40f, 0.05f, 0.07f, lane, hp, 0.0f
        );

        T* result = player.get();
        result->setActionState(ActionState::Moving);

        manager.players.push_back(std::move(player));
        return result;
    }
};

int main() {

    // TEST 1: Melee damages enemy
    {
        TestGame game;
        game.addPlayer<MeleePlayer>();

        HardEnemy enemy(0, {0.34f, 0.40f}, 2.0f);

        game.combat.update(0.0f, {&enemy});

        check(enemy.getHealth() == 6,
              "Melee player damages enemy");
    }

    // TEST 2: Ranged player creates a bullet
    {
        TestGame game;
        game.addPlayer<RangedPlayer>();

        game.combat.resolvePlayerAttacks({});

        check(game.manager.getActiveProjectiles().size() == 1,
              "Ranged player shoots automatically");
    }

    // TEST 3: Player bullet damages enemy once
    {
        TestGame game;
        auto* ranged = game.addPlayer<RangedPlayer>();

        game.manager.addProjectile(
            std::make_unique<Projectile>(
                0.41f, 0.42f, 0.012f, 0.012f,
                0, 0.5f, ranged
            )
        );

        EasyEnemy enemy(0, {0.40f, 0.40f}, 2.0f);

        game.combat.resolvePlayerProjectiles({&enemy});
        const int afterFirstHit = enemy.getHealth();

        game.combat.resolvePlayerProjectiles({&enemy});

        check(afterFirstHit == 4,
              "Player bullet deals 1 damage");

        check(enemy.getHealth() == afterFirstHit,
              "Player bullet cannot hit twice");
    }

    // TEST 4: High attack damages standing player
    {
        TestGame game;
        auto* player = game.addPlayer<MeleePlayer>();

        EnemyProjectile shot(
            0.31f, 0.42f, 0, 0.15f, 1, AttackType::High
        );

        game.combat.resolveEnemyProjectiles({&shot});

        check(player->getHealth() == 4 && !shot.isActive(),
              "High bullet damages standing player once");
    }

    // TEST 5: Duck avoids high attack
    {
        TestGame game;
        auto* player = game.addPlayer<MeleePlayer>();

        player->duck();

        EnemyProjectile shot(
            0.31f, 0.42f, 0, 0.15f, 1, AttackType::High
        );

        game.combat.resolveEnemyProjectiles({&shot});

        check(player->getHealth() == 5,
              "Ducking avoids high bullet");
    }

    // TEST 6: Jump avoids low attack
    {
        TestGame game;
        auto* player = game.addPlayer<MeleePlayer>();

        player->jump();

        EnemyProjectile shot(
            0.31f, 0.42f, 0, 0.15f, 1, AttackType::Low
        );

        game.combat.resolveEnemyProjectiles({&shot});

        check(player->getHealth() == 5,
              "Jumping avoids low bullet");
    }

    // TEST 7: Far enemy cannot cause instant damage
    {
        TestGame game;
        auto* player = game.addPlayer<MeleePlayer>();

        EasyEnemy enemy(0, {0.80f, 0.40f}, 2.0f);

        game.combat.resolveEnemyAttacks({&enemy});

        check(player->getHealth() == 5,
              "Far enemy cannot damage without bullet collision");
    }

    // TEST 8: Defense absorbs enemy bullet
    {
        TestGame game;

        auto* player = game.addPlayer<MeleePlayer>(0.30f);
        auto* defense = game.addPlayer<DefensePlayer>(0.50f, 0, 7);

        EnemyProjectile shot(
            0.31f, 0.42f, 0, 0.15f, 1, AttackType::High
        );

        game.combat.resolveEnemyProjectiles({&shot});

        check(player->getHealth() == 5 &&
              defense->getHealth() == 6,
              "Defense absorbs damage for protected player");
    }

    // TEST 9: Defense absorbs melee damage
    {
        TestGame game;

        auto* melee = game.addPlayer<MeleePlayer>(0.30f);
        auto* defense = game.addPlayer<DefensePlayer>(0.50f, 0, 7);

        HardEnemy enemy(0, {0.34f, 0.40f}, 2.0f);

        game.combat.resolveEnemyAttacks({&enemy});

        check(melee->getHealth() == 5 &&
              defense->getHealth() == 5 &&
              !enemy.canAttack(),
              "Defense absorbs melee damage");
    }

    // TEST 10: Collision blocks movement during cooldown
    {
        TestGame game;

        auto* player = game.addPlayer<MeleePlayer>(0.30f);
        player->setActionState(ActionState::Attacking);

        EasyEnemy enemy(0, {0.34f, 0.40f}, 2.0f);

        game.combat.update(0.0f, {&enemy});

        check(player->isMovementBlocked() &&
              enemy.isMovementBlocked(),
              "Collision blocks movement during attack cooldown");
    }

    // TEST 11: Bullet cannot hit a player in another lane
    {
        TestGame game;

        auto* player = game.addPlayer<MeleePlayer>(0.30f, 0, 5);

        EnemyProjectile shot(
            0.31f, 0.42f, 1, 0.15f, 1, AttackType::High
        );

        game.combat.resolveEnemyProjectiles({&shot});

        check(player->getHealth() == 5 && shot.isActive(),
              "Bullet cannot hit player in another lane");
    }

    // TEST 12: One bullet damages only one enemy
    {
        TestGame game;

        auto* ranged = game.addPlayer<RangedPlayer>();

        game.manager.addProjectile(
            std::make_unique<Projectile>(
                0.41f, 0.42f, 0.012f, 0.012f,
                0, 0.5f, ranged
            )
        );

        EasyEnemy enemy1(0, {0.40f, 0.40f}, 2.0f);
        EasyEnemy enemy2(0, {0.40f, 0.40f}, 2.0f);

        game.combat.resolvePlayerProjectiles({&enemy1, &enemy2});

        check(enemy1.getHealth() == 4 &&
              enemy2.getHealth() == 5,
              "One bullet damages only one enemy");
    }

    // TEST 13: One enemy bullet damages only one player
    {
        TestGame game;

        auto* player1 = game.addPlayer<MeleePlayer>(0.30f);
        auto* player2 = game.addPlayer<MeleePlayer>(0.30f);

        EnemyProjectile shot(
            0.31f, 0.42f, 0, 0.15f, 1, AttackType::High
        );

        game.combat.resolveEnemyProjectiles({&shot});

        check(player1->getHealth() == 4 &&
              player2->getHealth() == 5 &&
              !shot.isActive(),
              "One enemy bullet damages only one player");
    }

    // TEST 14: Inactive enemy bullet does not damage player
    {
        TestGame game;

        auto* player = game.addPlayer<MeleePlayer>();

        EnemyProjectile shot(
            0.31f, 0.42f, 0, 0.15f, 1, AttackType::High
        );

        shot.setActive(false);

        game.combat.resolveEnemyProjectiles({&shot});

        check(player->getHealth() == 5,
              "Inactive enemy bullet causes no damage");
    }

    // TEST 15: Enemy bullet misses distant player
    {
        TestGame game;

        auto* player = game.addPlayer<MeleePlayer>();

        EnemyProjectile shot(
            0.80f, 0.42f, 0, 0.15f, 1, AttackType::High
        );

        game.combat.resolveEnemyProjectiles({&shot});

        check(player->getHealth() == 5 &&
              shot.isActive(),
              "Enemy bullet misses distant player");
    }

    // TEST 16: Player bullet cannot damage enemy in another lane
    {
        TestGame game;

        auto* ranged = game.addPlayer<RangedPlayer>();

        game.manager.addProjectile(
            std::make_unique<Projectile>(
                0.41f, 0.42f, 0.012f, 0.012f,
                0, 0.5f, ranged
            )
        );

        EasyEnemy enemy(1, {0.40f, 0.40f}, 2.0f);

        game.combat.resolvePlayerProjectiles({&enemy});

        check(enemy.getHealth() == 5,
              "Player bullet cannot damage enemy in another lane");
    }

    // TEST 17: Player bullet misses distant enemy
    {
        TestGame game;

        auto* ranged = game.addPlayer<RangedPlayer>();

        game.manager.addProjectile(
            std::make_unique<Projectile>(
                0.41f, 0.42f, 0.012f, 0.012f,
                0, 0.5f, ranged
            )
        );

        EasyEnemy enemy(0, {0.80f, 0.40f}, 2.0f);

        game.combat.resolvePlayerProjectiles({&enemy});

        check(enemy.getHealth() == 5,
              "Player bullet misses distant enemy");
    }

    // TEST 18: Melee player cannot hit enemy in another lane
    {
        TestGame game;

        game.addPlayer<MeleePlayer>(0.30f, 0);

        EasyEnemy enemy(1, {0.34f, 0.40f}, 2.0f);

        game.combat.resolvePlayerAttacks({&enemy});

        check(enemy.getHealth() == 5,
              "Melee player cannot hit enemy in another lane");
    }

    // TEST 19: Melee player cannot hit distant enemy
    {
        TestGame game;

        game.addPlayer<MeleePlayer>(0.30f);

        EasyEnemy enemy(0, {0.80f, 0.40f}, 2.0f);

        game.combat.resolvePlayerAttacks({&enemy});

        check(enemy.getHealth() == 5,
              "Melee player cannot hit distant enemy");
    }

    // TEST 20: One melee attack damages only one enemy
    {
        TestGame game;

        game.addPlayer<MeleePlayer>(0.30f);

        EasyEnemy enemy1(0, {0.34f, 0.40f}, 2.0f);
        EasyEnemy enemy2(0, {0.34f, 0.40f}, 2.0f);

        game.combat.resolvePlayerAttacks({&enemy1, &enemy2});

        check(enemy1.getHealth() == 4 &&
              enemy2.getHealth() == 5,
              "One melee attack damages only one enemy");
    }

    // TEST 21: Enemy cannot attack twice during cooldown
    {
        TestGame game;

        auto* player = game.addPlayer<MeleePlayer>(0.30f);

        HardEnemy enemy(0, {0.34f, 0.40f}, 2.0f);

        game.combat.resolveEnemyAttacks({&enemy});
        game.combat.resolveEnemyAttacks({&enemy});

        check(player->getHealth() == 3 &&
              !enemy.canAttack(),
              "Enemy cannot attack twice during cooldown");
    }

    // TEST 22: Defender cannot protect player in another lane
    {
        TestGame game;

        auto* player = game.addPlayer<MeleePlayer>(0.30f, 0, 5);
        auto* defense = game.addPlayer<DefensePlayer>(0.50f, 1, 7);

        EnemyProjectile shot(
            0.31f, 0.42f, 0, 0.15f, 1, AttackType::High
        );

        game.combat.resolveEnemyProjectiles({&shot});

        check(player->getHealth() == 4 &&
              defense->getHealth() == 7,
              "Defender cannot protect another lane");
    }

    // TEST 23: Disabled defense cannot absorb damage
    {
        TestGame game;

        auto* player = game.addPlayer<MeleePlayer>(0.30f);
        auto* defense = game.addPlayer<DefensePlayer>(0.50f, 0, 7);

        defense->disableProtection();

        EnemyProjectile shot(
            0.31f, 0.42f, 0, 0.15f, 1, AttackType::High
        );

        game.combat.resolveEnemyProjectiles({&shot});

        check(player->getHealth() == 4 &&
              defense->getHealth() == 7,
              "Disabled defender cannot absorb damage");
    }

    // TEST 24: No collision means no movement blocking
    {
        TestGame game;

        auto* player = game.addPlayer<MeleePlayer>(0.30f);

        EasyEnemy enemy(0, {0.80f, 0.40f}, 2.0f);

        game.combat.update(0.0f, {&enemy});

        check(!player->isMovementBlocked() &&
              !enemy.isMovementBlocked(),
              "No collision means movement is not blocked");
    }


    // A fresh game and a restart both use the real economy constructor.
    {
        Economy economy;
        check(economy.getCoins() == 30, "New game starts with 30 coins");
        check(economy.spendCoins(10) && economy.spendCoins(10) &&
              economy.spendCoins(10) && !economy.spendCoins(10),
              "Starting coins buy exactly three 10-coin melee players");
        economy = Economy();
        check(economy.getCoins() == 30, "Restart restores 30 coins");
    }

    // Simulate an actual unprotected melee duel with production attack timing.
    // Cover different frame rates so the result cannot depend on one timestep.
    for (float dt : {1.0f / 30.0f, 1.0f / 60.0f, 1.0f / 144.0f}) {
        TestGame game;
        auto* player = game.addPlayer<MeleePlayer>();
        HardEnemy enemy(0, {0.34f, 0.40f}, GameConfig::HARD_ENEMY_ATTACK_INTERVAL);
        for (int frame = 0; frame < static_cast<int>(10.0f / dt) &&
             player->isAlive() && enemy.getHealth() > 0; ++frame) {
            enemy.update(dt);
            game.manager.update(dt);
            game.combat.update(dt, {&enemy});
        }
        check(!player->isAlive() && enemy.getHealth() > 0,
              "Hard enemy defeats one unprotected starter melee player");
    }

    {
        EasyEnemy easy(0, {0.50f, 0.40f}, GameConfig::EASY_ENEMY_ATTACK_INTERVAL);
        MediumEnemy medium(0, {0.50f, 0.40f}, GameConfig::MEDIUM_ENEMY_ATTACK_INTERVAL);
        HardEnemy hard(0, {0.50f, 0.40f}, GameConfig::HARD_ENEMY_ATTACK_INTERVAL);
        check(easy.getHealth() == 5 && medium.getHealth() == 5 && hard.getHealth() == 7 &&
              easy.getDamage() == 1 && medium.getDamage() == 1 && hard.getDamage() == 2,
              "Enemy health and damage match the project description");
        check(easy.getSpeed() > 0.0f && easy.getSpeed() == medium.getSpeed() &&
              hard.getSpeed() > easy.getSpeed(),
              "Easy and Medium share normal speed; Hard runs faster");
        WaveManager waves;
        check(waves.spawnEnemyProjectile(&easy) && waves.spawnEnemyProjectile(&medium) &&
              !waves.spawnEnemyProjectile(&hard),
              "Ranged enemies can shoot immediately; Hard only attacks in melee");
        auto* high = waves.getEnemyProjectileAt(0);
        auto* low = waves.getEnemyProjectileAt(1);
        check(high && low && high->getAttackType() == AttackType::High &&
              low->getAttackType() == AttackType::Low && high->getDamage() == 1 &&
              low->getDamage() == 1,
              "Easy shoots high and Medium shoots low, each dealing 1 damage");
    }

    {
        TestGame game;
        auto* player = game.addPlayer<RangedPlayer>();
        EasyEnemy enemy(0, {0.34f, 0.40f}, GameConfig::EASY_ENEMY_ATTACK_INTERVAL);
        game.combat.update(0.0f, {&enemy});
        game.combat.update(0.0f, {&enemy});
        check(player->isMovementBlocked() && enemy.isMovementBlocked(),
              "Colliding ranged units stay stopped during attack cooldowns");
        enemy.takeDamage(5);
        game.combat.update(0.0f, {&enemy});
        check(!player->isMovementBlocked(), "Survivor resumes movement after enemy dies");
    }

    // Verify the production placement path uses the required player stats.
    {
        Grid grid;
        GameManager manager;
        manager.setGrid(&grid);
        Player* melee = manager.placePlayer(PlayerType::Melee, 0);
        Player* ranged = manager.placePlayer(PlayerType::Ranged, 1);
        Player* defense = manager.placePlayer(PlayerType::Defense, 2);
        check(melee && ranged && defense, "All player types can be placed on valid lanes");
        if (melee && ranged && defense) {
            check(melee->getHealth() == 5 && melee->getMaxHealth() == 5 &&
                  ranged->getHealth() == 5 && ranged->getMaxHealth() == 5 &&
                  defense->getHealth() == 7 && defense->getMaxHealth() == 7,
                  "Placed players have 5 melee, 5 ranged, and 7 defense HP");
            check(melee->getMoveSpeed() > 0.0f && ranged->getMoveSpeed() > 0.0f &&
                  defense->getMoveSpeed() == 0.0f && !defense->canAttack(),
                  "Melee and ranged advance; Defense is stationary and cannot attack");
            const float meleeX = melee->getX();
            const float rangedX = ranged->getX();
            const float defenseX = defense->getX();
            manager.update(1.0f);
            check(melee->getX() > meleeX && ranged->getX() > rangedX &&
                  defense->getX() == defenseX,
                  "Production player movement matches the description");
        }
    }

    {
        TestGame game;
        auto* ranged = game.addPlayer<RangedPlayer>();
        ranged->startAttack(game.manager);
        ranged->finishAttack();
        ranged->update(0.5f);
        check(!ranged->canAttack(), "Ranged cooldown no longer uses the old 0.5-second value");
        ranged->update(GameConfig::RANGED_ATTACK_INTERVAL - 0.5f);
        check(ranged->canAttack(), "Ranged attack becomes ready at its configured cooldown");
    }

    std::cout << "\nPassed: " << passed
              << " | Failed: " << failed << '\n';

    return failed == 0 ? 0 : 1;
}
