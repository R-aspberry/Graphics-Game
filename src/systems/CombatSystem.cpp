#include "CombatSystem.h"
#include "CollisionSystem.h"
#include "GameManager.h"
#include "WaveManager.h"
#include "../entities/DefensePlayer.h"
#include "../entities/MeleePlayer.h"
#include "../entities/RangedPlayer.h"
#include "../utils/WorldLayout.h"
#include <algorithm>
#include <cmath>

namespace {
bool alive(const Enemy* enemy) {
    return enemy && enemy->getHealth() > 0 && enemy->getState() != EnemyState::DEAD;
}
bool alive(const Player* player) {
    return player && player->active && player->isAlive();
}
Rect enemyBounds(const Enemy& enemy) {
    return {enemy.getPosition().x, enemy.getPosition().y,
            WorldLayout::ENEMY_WIDTH, WorldLayout::ENEMY_HEIGHT};
}
// Cover this frame's travel so fast shots do not tunnel through stationary targets.
Rect shotPath(float x, float y, float width, float height, float travel) {
    return {std::min(x, x - travel), y, width + std::fabs(travel), height};
}
}

CombatSystem::CombatSystem(GameManager* manager, CollisionSystem* collisions)
    : playerManager(manager), collisionSystem(collisions) {}

void CombatSystem::setPlayerManager(GameManager* manager) { playerManager = manager; }
void CombatSystem::setEnemies(const std::vector<Enemy*>& activeEnemies) { enemies = activeEnemies; }

void CombatSystem::update(float deltaTime, const std::vector<Enemy*>& activeEnemies) {
    update(deltaTime, activeEnemies, {});
}

void CombatSystem::update(float deltaTime, WaveManager& waves) {
    std::vector<Enemy*> activeEnemies;
    std::vector<EnemyProjectile*> shots;
    for (std::size_t i = 0; i < waves.getActiveEnemyCount(); ++i)
        activeEnemies.push_back(waves.getEnemyAt(i));
    for (std::size_t i = 0; i < waves.getEnemyProjectilesCount(); ++i)
        shots.push_back(waves.getEnemyProjectileAt(i));
    update(deltaTime, activeEnemies, shots);
}

void CombatSystem::update(float deltaTime, const std::vector<Enemy*>& activeEnemies,
                          const std::vector<EnemyProjectile*>& shots) {
    if (!std::isfinite(deltaTime) || deltaTime < 0.0f || !playerManager || !collisionSystem) return;
    setEnemies(activeEnemies);
    updateEncounters(activeEnemies);
    // Resolve existing player shots before creating new ones, which have not moved.
    resolvePlayerProjectiles(deltaTime, activeEnemies);
    resolveEnemyProjectiles(deltaTime, shots);
    resolvePlayerAttacks(activeEnemies);
    resolveEnemyAttacks(activeEnemies);
    // Release survivors immediately when their opponent dies in this step.
    updateEncounters(activeEnemies);
}

bool CombatSystem::inMeleeRange(const Player& player, const Enemy& enemy, float range) const {
    const Rect e = enemyBounds(enemy);
    constexpr float epsilon = 0.000001f;
    return alive(&player) && alive(&enemy) && player.laneIndex == enemy.getLane() &&
        player.y <= e.y + e.height && player.y + player.height >= e.y &&
        e.x + e.width >= player.x &&
        e.x <= player.x + player.width + std::max(0.0f, range) + epsilon;
}

void CombatSystem::updateEncounters(const std::vector<Enemy*>& activeEnemies) {
    if (!playerManager || !collisionSystem) return;
    const auto players = playerManager->getActivePlayers();
    for (Player* player : players) player->setMovementBlocked(false);
    for (Enemy* enemy : activeEnemies)
        if (enemy) enemy->setMovementBlocked(false);
    for (Player* player : players) {
        for (Enemy* enemy : activeEnemies) {
            // Ranged enemies also stop at contact.
            if (enemy && inMeleeRange(*player, *enemy)) {
                player->setMovementBlocked(true);
                enemy->setMovementBlocked(true);
            }
        }
    }
}

void CombatSystem::resolvePlayerAttacks(const std::vector<Enemy*>& activeEnemies) {
    if (!playerManager || !collisionSystem) return;
    for (Player* player : playerManager->getActivePlayers()) {
        if (!player->canAttack()) continue;
        if (auto* melee = dynamic_cast<MeleePlayer*>(player)) {
            Enemy* target = nullptr;
            for (Enemy* enemy : activeEnemies) {
                if (enemy && inMeleeRange(*melee, *enemy, melee->getAttackRange()) &&
                    (!target || enemy->getPosition().x < target->getPosition().x)) target = enemy;
            }
            if (target) resolvePlayerAttack(*player, *target);
            else player->startAttack(*playerManager);
        } else if (dynamic_cast<RangedPlayer*>(player)) {
            // Automatic shooting is required even in an empty lane.
            player->startAttack(*playerManager);
        }
    }
}

void CombatSystem::resolvePlayerAttack(Player& player, Enemy& enemy) {
    if (!playerManager || !collisionSystem || !alive(&player) ||
        !alive(&enemy) || !player.canAttack()) return;
    if (auto* melee = dynamic_cast<MeleePlayer*>(&player)) {
        if (!inMeleeRange(player, enemy, melee->getAttackRange())) return;
        player.startAttack(*playerManager);
        // Damage once at the beginning of a swing, never once per render frame.
        if (melee->isAttacking()) enemy.takeDamage(melee->getAttackDamage());
    } else if (dynamic_cast<RangedPlayer*>(&player) &&
               player.laneIndex == enemy.getLane() && enemy.getPosition().x >= player.x) {
        player.startAttack(*playerManager);
    }
}

DefensePlayer* CombatSystem::findProtector(const Player& player) const {
    if (!playerManager || !alive(&player)) return nullptr;
    DefensePlayer* protector = nullptr;
    for (Player* candidate : playerManager->getActivePlayers()) {
        auto* defense = dynamic_cast<DefensePlayer*>(candidate);
        if (defense && defense->protects(player) &&
            (!protector || defense->x > protector->x)) protector = defense;
    }
    return protector;
}

bool CombatSystem::isPlayerProtected(const Player& player, const std::vector<Enemy*>&) const {
    return findProtector(player) != nullptr;
}

void CombatSystem::resolveEnemyAttack(Enemy& enemy, Player& player) {
    if (!playerManager || !collisionSystem || !alive(&player) ||
        enemy.getAttackType() != AttackType::Melee || !enemy.canAttack() ||
        !inMeleeRange(player, enemy)) return;
    Player* recipient = &player;
    if (auto* defense = findProtector(player)) {
        // A defender beyond the enemy cannot intercept its melee weapon.
        if (defense->x <= enemy.getPosition().x + WorldLayout::ENEMY_WIDTH) recipient = defense;
    }
    recipient->takeDamage(enemy.getDamage());
    enemy.resetAttackTimer();
}

void CombatSystem::resolveEnemyAttacks(const std::vector<Enemy*>& activeEnemies) {
    if (!playerManager || !collisionSystem) return;
    const auto players = playerManager->getActivePlayers();
    for (Enemy* enemy : activeEnemies) {
        if (!alive(enemy) || enemy->getAttackType() != AttackType::Melee) continue;
        Player* target = nullptr;
        for (Player* player : players) {
            if (inMeleeRange(*player, *enemy) && (!target || player->x > target->x)) target = player;
        }
        if (target) resolveEnemyAttack(*enemy, *target);
    }
    // High/low enemy damage is applied only by projectile collision.
}

void CombatSystem::resolvePlayerProjectiles(float deltaTime, const std::vector<Enemy*>& activeEnemies) {
    if (!playerManager || !collisionSystem || !std::isfinite(deltaTime) || deltaTime < 0) return;
    for (Projectile* shot : playerManager->getActiveProjectiles()) {
        if (shot->hasAlreadyHit()) continue;
        const Rect path = shotPath(shot->x, shot->y, shot->width, shot->height,
                                   shot->getSpeed() * deltaTime);
        Enemy* target = nullptr;
        for (Enemy* enemy : activeEnemies) {
            if (alive(enemy) && enemy->getLane() == shot->laneIndex &&
                collisionSystem->intersects(path, enemyBounds(*enemy)) &&
                (!target || enemy->getPosition().x < target->getPosition().x)) target = enemy;
        }
        if (target) {
            target->takeDamage(shot->getDamage());
            shot->markHit();
        } else if (shot->x > 1.0f || shot->x + shot->width < 0.0f) shot->setActive(false);
    }
}

void CombatSystem::resolveEnemyProjectiles(float deltaTime, const std::vector<EnemyProjectile*>& shots) {
    if (!playerManager || !collisionSystem || !std::isfinite(deltaTime) || deltaTime < 0) return;
    auto players = playerManager->getActivePlayers();
    // Leftward shots hit the rightmost vulnerable player first, regardless of purchase order.
    std::stable_sort(players.begin(), players.end(),
                     [](const Player* a, const Player* b) { return a->x > b->x; });
    for (EnemyProjectile* shot : shots) {
        if (!shot || !shot->isActive()) continue;
        const Rect path = shotPath(shot->x, shot->y, shot->width, shot->height,
                                   -shot->getSpeed() * deltaTime);
        for (Player* player : players) {
            if (!alive(player) || player->laneIndex != shot->laneIndex ||
                !collisionSystem->intersects(path, player->getBounds())) continue;
            if ((shot->getAttackType() == AttackType::High && player->isDucking()) ||
                (shot->getAttackType() == AttackType::Low && player->isJumping())) continue;
            Player* recipient = player;
            if (auto* defense = findProtector(*player)) recipient = defense;
            recipient->takeDamage(shot->getDamage());
            shot->setActive(false);
            break;
        }
        if (shot->x + shot->width < 0.0f || shot->x > 1.0f) shot->setActive(false);
    }
}
