#ifndef MELEE_PLAYER_H
#define MELEE_PLAYER_H

#include "Player.h"

/**
 * MeleePlayer specializes in close-range engagement.
 * Implements high-damage, short-range attacks with a specific cooldown cycle.
 */
class MeleePlayer : public Player {
private:
    int attackDamage;
    float attackRange;
    float attackCoolDownTimer;
    float attackTimer;

public:
    MeleePlayer(float x, float y, float width, float height, int laneIndex, float moveSpeed, int maxHealth);
    virtual ~MeleePlayer() override;

    // Player Interface Overrides
    virtual PlayerType getPlayerType() const override;
    virtual void startAttack() override;
    virtual bool canAttack() const override;

    // Combat mechanics
    void updateAttack(float deltaTime);
    void finishAttack();

    int getAttackDamage() const;
    float getAttackRange() const;
    bool isAttacking() const;

    void update(float deltaTime);
};

#endif // MELEE_PLAYER_H
