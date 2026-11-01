#ifndef RANGED_PLAYER_H
#define RANGED_PLAYER_H

#include "Player.h"
#include "Projectile.h"
#include "GameManager.h"

class RangedPlayer : public Player {
private:
    int attackDamage = 1 ; // not mentioned 
    //float attackRange = 0.6f; //i still dk ??
    float attackCoolDownTimer = 0.0f;
    float attackTimer = 0.0f;
    //float attackSpeed = 0.3f;
    //bool attacking;

public:
    RangedPlayer(float x, float y, float width, float height, int laneIndex,  int maxHealth, float moveSpeed);
    virtual ~RangedPlayer() override;

    // Player Interface Implementations
    virtual PlayerType getPlayerType() const override;
    virtual void startAttack(GameManager& gameManager) override;
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
