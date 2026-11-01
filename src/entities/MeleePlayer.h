#ifndef MELEE_PLAYER_H
#define MELEE_PLAYER_H

#include "Player.h"

class MeleePlayer : public Player {
private:
     int attackDamage = 1 ;
    //float attackRange = 0.3f; //i still dk ??
    float attackCoolDownTimer = 0.0f;
    float attackTimer = 0.0f;
    //bool attacking;

public:
    MeleePlayer(float x, float y, float width, float height, int laneIndex, int maxHealth, float moveSpeed);
    virtual ~MeleePlayer() override;

    PlayerType getPlayerType() const override;
    void startAttack(GameManager &gameManager ) override;
    bool canAttack() const override;
    void updateAttack(float deltaTime);
    void finishAttack();

    int getAttackDamage() const;
    float getAttackRange() const;
    bool isAttacking() const;

    void update(float deltaTime) override;
};

#endif
