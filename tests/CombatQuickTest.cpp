
#include "../src/systems/CombatSystem.h"
#include "../src/systems/CollisionSystem.h"
#include "../src/systems/GameManager.h"
#include "../src/entities/MeleePlayer.h"
#include "../src/entities/RangedPlayer.h"
#include "../src/entities/DefensePlayer.h"
#include "../src/entities/EasyEnemy.h"
#include "../src/entities/HardEnemy.h"
#include "../src/entities/EnemyProjectile.h"

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


    std::cout << "\nPassed: " << passed
              << " | Failed: " << failed << '\n';

    return failed == 0 ? 0 : 1;
}
