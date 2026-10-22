#ifndef RANGED_PLAYER_H
#define RANGED_PLAYER_H

#include "Player.h"
#include "Projectile.h"

/**
 * RangedPlayer specializes in long-distance combat using Projectiles.
 * Manages attack cooldowns and projectile spawning logic.
 */
class RangedPlayer : public Player {
private:
    int attackDamage;
    float attackRange;
    float attackCoolDownTimer;
    float attackTimer;

public:
    RangedPlayer(float x, float y, float width, float height, int laneIndex, float moveSpeed, int maxHealth);
    virtual ~RangedPlayer() override;

    // Player Interface Implementations
    virtual PlayerType getPlayerType() const override;
    virtual void startAttack() override;
    virtual bool canAttack() const override;

    // Ranged-specific mechanics
    void updateAttack(float deltaTime);
    void finishAttack();

    int getAttackDamage() const;
    float getAttackRange() const;
    bool isAttacking() const;

    void update(float deltaTime);
};

#endif // RANGED_PLAYER_H
