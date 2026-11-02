#include "../src/systems/CombatSystem.h"
#include "../src/systems/CombatSystem.h" // Header must support repeated inclusion.
#include "../src/systems/CollisionSystem.h"
#include "../src/systems/GameManager.h"
#include "../src/systems/WaveManager.h"
#include "../src/entities/MeleePlayer.h"
#include "../src/entities/RangedPlayer.h"
#include "../src/entities/DefensePlayer.h"
#include "../src/entities/HardEnemy.h"
#include "../src/entities/EasyEnemy.h"
#include "../src/utils/Constants.h"
#include <iostream>
#include <limits>
#include <memory>
#include <string>

int passed = 0, failed = 0;
void check(bool result, const std::string& name) {
    (result ? ++passed : ++failed);
    std::cout << (result ? "[PASS] " : "[FAIL] ") << name << '\n';
}
struct Fixture {
    GameManager manager;
    CollisionSystem collision;
    CombatSystem combat{&manager, &collision};
    template<class T> T* add(float x = 0.3f, int lane = 0, int health = 20) {
        auto player = std::make_unique<T>(x, 0.4f, WorldLayout::PLAYER_WIDTH,
                                          WorldLayout::PLAYER_HEIGHT, lane, health, 0.0f);
        T* result = player.get();
        player->setActionState(ActionState::Moving);
        manager.players.push_back(std::move(player));
        return result;
    }
};
void encountersAndMelee() {
    Fixture f;
    auto* melee = f.add<MeleePlayer>();
    HardEnemy enemy(0, {0.345f, 0.4f}, 2.0f);
    f.combat.update(0.0f, {nullptr, &enemy});
    check(enemy.getHealth() == 6, "Melee swing deals one damage");
    check(melee->getHealth() == 18, "Hard enemy deals two damage at contact");
    check(melee->isMovementBlocked() && enemy.isMovementBlocked(), "Both characters stop at contact");
    f.combat.update(0.0f, {&enemy});
    check(enemy.getHealth() == 6 && melee->getHealth() == 18, "Repeated frames do not repeat hits during cooldown");
    enemy.takeDamage(100);
    f.combat.update(0.0f, {&enemy});
    check(!melee->isMovementBlocked(), "Survivor is released after enemy death");
    HardEnemy another(0, {0.345f, 0.4f}, 2.0f);
    f.combat.updateEncounters({&another});
    melee->takeDamage(100);
    f.combat.updateEncounters({&another});
    check(!another.isMovementBlocked(), "Enemy is released after player death");

    Fixture lanes;
    auto* p = lanes.add<MeleePlayer>();
    HardEnemy otherLane(1, {0.34f, 0.4f}, 2.0f);
    HardEnemy far(0, {0.8f, 0.4f}, 2.0f);
    lanes.combat.update(0.0f, {&otherLane, &far});
    check(otherLane.getHealth() == 7 && far.getHealth() == 7 && p->getHealth() == 20,
          "Neither other lanes nor distant melee targets take damage");
    check(p->isAttacking(), "Melee animates attacks even with no nearby target");

    Fixture edge;
    auto* q = edge.add<MeleePlayer>();
    EasyEnemy touching(0, {q->x + q->width, 0.4f}, 2.0f);
    edge.combat.update(0.0f, {&touching});
    check(touching.getHealth() == 4 && touching.isMovementBlocked() && q->isMovementBlocked(),
          "Exact contact blocks ranged enemies and permits melee damage");
    check(q->getHealth() == 20, "High ranged enemy cannot apply instant direct damage");

    Fixture lethal;
    auto* finisher = lethal.add<MeleePlayer>();
    HardEnemy weak(0, {0.34f, 0.4f}, 2.0f);
    weak.takeDamage(6);
    lethal.combat.update(0, {&weak});
    check(weak.getHealth() == 0 && finisher->getHealth() == 20 && !finisher->isMovementBlocked(),
          "Lethal swing prevents dead enemy retaliation and releases movement");
}
void rangedAndPlayerShots() {
    Fixture f;
    auto* ranged = f.add<RangedPlayer>();
    f.combat.update(0.0f, std::vector<Enemy*>{});
    check(f.manager.getActiveProjectiles().size() == 1, "Ranged player fires automatically into empty lane");
    f.combat.update(0.0f, std::vector<Enemy*>{});
    check(f.manager.getActiveProjectiles().size() == 1, "Attack animation prevents duplicate shots");
    ranged->update(Constants::attackDuration);
    check(!ranged->canAttack(), "Ranged cooldown uses configured interval");
    ranged->update(GameConfig::RANGED_ATTACK_INTERVAL);
    check(ranged->canAttack(), "Ranged player can attack after cooldown");

    Fixture shots;
    auto* owner = shots.add<RangedPlayer>();
    shots.manager.addProjectile(std::make_unique<Projectile>(0.36f, 0.42f, 0.012f, 0.012f, 0, 0.5f, owner));
    EasyEnemy near(0, {0.5f, 0.4f}, 2.0f);
    EasyEnemy far(0, {0.7f, 0.4f}, 2.0f);
    EasyEnemy otherLane(1, {0.45f, 0.4f}, 2.0f);
    // Owner may be destroyed before its projectile reaches the enemy.
    owner->takeDamage(100);
    shots.manager.removeDefeatedPlayers();
    shots.manager.updateProjectiles(1.0f);
    shots.combat.resolvePlayerProjectiles(1.0f, {&far, &otherLane, nullptr, &near});
    check(near.getHealth() == 4 && far.getHealth() == 5 && otherLane.getHealth() == 5,
          "Swept shot hits first enemy in its lane after owner destruction");
    check(shots.manager.projectiles[0]->hasAlreadyHit() && !shots.manager.projectiles[0]->isActive(),
          "Colliding player shot is consumed");
    shots.combat.resolvePlayerProjectiles(1.0f, {&near});
    check(near.getHealth() == 4, "Consumed shot cannot damage twice");
    shots.manager.cleanupInactiveEntities();
    check(shots.manager.projectiles.empty(), "Manager can clean up consumed player shots");
    shots.manager.addProjectile(std::make_unique<Projectile>(1.1f, 0.42f, .012f, .012f, 0, .5f, nullptr));
    shots.combat.resolvePlayerProjectiles(0, {});
    check(shots.manager.getActiveProjectiles().empty(), "Missed player shots expire off screen");
}
void enemyShotsAndDefense() {
    Fixture f;
    auto* p = f.add<MeleePlayer>();
    EnemyProjectile high(.31f, .42f, 0, .15f, 1, AttackType::High);
    p->duck();
    f.combat.resolveEnemyProjectiles(0, {nullptr, &high});
    check(p->getHealth() == 20 && high.isActive(), "Ducking avoids high projectile without consuming it");
    p->setActionState(ActionState::Moving);
    f.combat.resolveEnemyProjectiles(0, {&high});
    check(p->getHealth() == 19 && !high.isActive(), "High projectile damages standing player once");
    f.combat.resolveEnemyProjectiles(0, {&high});
    check(p->getHealth() == 19, "Consumed enemy shot cannot hit twice");
    p->updateDamageFeedback(1);
    p->jump();
    EnemyProjectile low(.31f, .42f, 0, .15f, 1, AttackType::Low);
    f.combat.resolveEnemyProjectiles(0, {&low});
    check(p->getHealth() == 19 && low.isActive(), "Jumping avoids low projectile");
    EnemyProjectile wrong(.31f, .42f, 1, .15f, 1, AttackType::High);
    f.combat.resolveEnemyProjectiles(0, {&wrong});
    check(wrong.isActive(), "Enemy shots cannot damage another lane");
    p->setActionState(ActionState::Moving);
    p->duck();
    f.combat.resolveEnemyProjectiles(0, {&low});
    check(p->getHealth() == 18 && !low.isActive(), "Ducking does not avoid low attack");
    EnemyProjectile again(.31f, .42f, 0, .15f, 1, AttackType::High);
    f.combat.resolveEnemyProjectiles(0, {&again});
    p->updateDamageFeedback(1);
    check(p->getActionState() != ActionState::Damaged, "Repeated hits recover from damage feedback");

    Fixture shield;
    auto* behind = shield.add<MeleePlayer>(.3f);
    auto* defense = shield.add<DefensePlayer>(.5f, 0, 7);
    auto* other = shield.add<DefensePlayer>(.8f, 1, 7);
    check(shield.combat.isPlayerProtected(*behind, {}), "Defense protects living players behind it");
    // Direct collision at a protected player must still charge damage to the shield.
    EnemyProjectile protectedShot(.31f, .42f, 0, .15f, 1, AttackType::High);
    shield.combat.resolveEnemyProjectiles(0, {&protectedShot});
    check(behind->getHealth() == 20 && defense->getHealth() == 6 && other->getHealth() == 7,
          "Defender absorbs protected hit and loses health");
    defense->disableProtection();
    check(!shield.combat.isPlayerProtected(*behind, {}), "Disabled defender provides no protection");
    EnemyProjectile exposed(.31f, .42f, 0, .15f, 1, AttackType::High);
    shield.combat.resolveEnemyProjectiles(0, {&exposed});
    check(behind->getHealth() == 19, "Player becomes vulnerable when protection is disabled");
    behind->x = .6f;
    check(!shield.combat.isPlayerProtected(*behind, {}), "Passing defender ends protection");

    Fixture order;
    auto* rear = order.add<MeleePlayer>(.3f);
    auto* front = order.add<DefensePlayer>(.6f, 0, 1);
    EnemyProjectile swept(.2f, .42f, 0, .5f, 1, AttackType::High);
    order.combat.resolveEnemyProjectiles(1, {&swept});
    check(front->getHealth() == 0 && rear->getHealth() == 20 && !swept.isActive(),
          "Swept enemy shot hits front defender regardless of purchase order");
    check(!order.combat.isPlayerProtected(*rear, {}), "Defeated defender immediately loses protection");
    EnemyProjectile next(.31f, .42f, 0, .15f, 1, AttackType::High);
    order.combat.resolveEnemyProjectiles(0, {&next});
    check(rear->getHealth() == 19, "Next shot hits exposed player after defender death");

    Fixture melee;
    auto* back = melee.add<MeleePlayer>(.3f);
    auto* blocker = melee.add<DefensePlayer>(.34f, 0, 7);
    HardEnemy hard(0, {.345f, .4f}, 2);
    melee.combat.resolveEnemyAttacks({&hard});
    check(back->getHealth() == 20 && blocker->getHealth() == 5 && !hard.canAttack(),
          "Hard enemy hits foremost defender once and consumes cooldown");
}
void guardsAndWaveIntegration() {
    Fixture f;
    auto* p = f.add<MeleePlayer>();
    HardEnemy enemy(0, {.34f, .4f}, 2);
    f.combat.update(-1, {&enemy});
    f.combat.update(std::numeric_limits<float>::quiet_NaN(), {&enemy});
    f.combat.update(std::numeric_limits<float>::infinity(), {&enemy});
    check(enemy.getHealth() == 7 && p->getHealth() == 20, "Invalid simulation times have no combat side effects");
    CombatSystem detached(nullptr, nullptr);
    detached.update(0, {&enemy});
    detached.resolvePlayerAttack(*p, enemy);
    detached.resolveEnemyAttack(enemy, *p);
    detached.updateEncounters({&enemy});
    detached.setEnemies({&enemy});
    detached.setPlayerManager(&f.manager);
    check(!detached.isPlayerProtected(*p, {}), "Missing dependencies and setter are safe");
    WaveManager waves;
    waves.update(GameConfig::ENEMY_SPAWN_INTERVAL);
    Enemy* spawned = waves.getEnemyAt(0);
    const auto pos = spawned->getPosition();
    auto* victim = f.add<MeleePlayer>(pos.x - .1f, spawned->getLane());
    victim->y = pos.y;
    // Enemy projectile moves into the player on the following manager update.
    waves.update(0);
    waves.update(.5f);
    f.combat.update(.5f, waves);
    check(victim->getHealth() == 19, "WaveManager overload resolves its real high projectiles");
}
void moreCombatBoundaries() {
    Fixture f;
    auto* melee = f.add<MeleePlayer>();
    HardEnemy first(0, {.34f, .4f}, 2);
    HardEnemy second(0, {.345f, .4f}, 2);
    f.combat.resolvePlayerAttacks({&second, &first});
    check(first.getHealth() == 6 && second.getHealth() == 7,
          "One swing hits only the nearest enemy regardless of vector order");
    melee->update(Constants::attackDuration);
    check(!melee->canAttack(), "Melee cannot swing again as soon as animation ends");
    melee->update(GameConfig::MELEE_ATTACK_INTERVAL);
    f.combat.resolvePlayerAttacks({&first});
    check(first.getHealth() == 5, "Melee deals another single hit after cooldown");
    f.combat.updateEncounters({&first, &second});
    first.takeDamage(100);
    f.combat.updateEncounters({&first, &second});
    check(melee->isMovementBlocked(), "Survivor stays blocked while a second opponent remains");
    second.takeDamage(100);
    f.combat.updateEncounters({&first, &second});
    check(!melee->isMovementBlocked(), "Survivor resumes when all touching enemies die");

    Fixture ranged;
    auto* shooter = ranged.add<RangedPlayer>();
    EasyEnemy distant(0, {.8f, .4f}, 2);
    ranged.combat.resolvePlayerAttack(*shooter, distant);
    check(distant.getHealth() == 5 && ranged.manager.projectiles.size() == 1,
          "Starting a ranged attack creates a shot without instant target damage");
    check(shooter->getHealth() == 20, "Player shot causes no friendly fire");
    ranged.combat.update(.25f, {&distant});
    check(shooter->isAttacking() && distant.getPosition().x == .8f &&
          distant.getTimeSinceLastAttack() == distant.getAttackInterval(),
          "Combat does not advance movement, animation or cooldown clocks");

    Fixture evade;
    auto* rear = evade.add<MeleePlayer>(.3f);
    auto* front = evade.add<MeleePlayer>(.6f);
    front->duck();
    EnemyProjectile high(.2f, .42f, 0, .5f, 1, AttackType::High);
    evade.combat.resolveEnemyProjectiles(1, {&high});
    check(front->getHealth() == 20 && rear->getHealth() == 19,
          "A dodged shot continues to a vulnerable player behind the dodger");
    Fixture jump;
    auto* jumper = jump.add<MeleePlayer>();
    jumper->jump();
    EnemyProjectile upper(.31f, .42f, 0, .15f, 1, AttackType::High);
    jump.combat.resolveEnemyProjectiles(0, {&upper});
    check(jumper->getHealth() == 19, "Jumping does not avoid high attacks");

    Fixture placement;
    Grid grid;
    placement.manager.setGrid(&grid);
    Player* placed = placement.manager.placePlayer(PlayerType::Melee, 0);
    check(placed && placed->width == WorldLayout::PLAYER_WIDTH &&
          placed->height == WorldLayout::PLAYER_HEIGHT,
          "Production placement uses normalized combat hitbox dimensions");
    placement.combat.update(0, {});
    check(placed->getActionState() == ActionState::Attacking,
          "Production-placed player enters automatic combat");
}
int main() {
    encountersAndMelee();
    rangedAndPlayerShots();
    enemyShotsAndDefense();
    guardsAndWaveIntegration();
    moreCombatBoundaries();
    std::cout << passed << " passed, " << failed << " failed\n";
    return failed ? 1 : 0;
}
