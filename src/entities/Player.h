#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.h"
#include "../utils/Types.h"
#include "../utils/Animation.h"

class GameManager;

class Player : public Entity {
public:
    int health;
    int maxHealth;
    float moveSpeed;
    ActionState actionState;
    ActionState PrevactionState;
    bool movementBlocked;
    float jumpTimer;
    float duckTimer;
    float damageFlashTimer;
    float attackRange;
    Animation animation;

public:
    Player(float x, float y, float width, float height, int laneIndex, int maxHealth, float moveSpeed);
    virtual ~Player() override;

    virtual void takeDamage(int damage);
    bool isAlive() const;

    int getHealth() const;
    int getMaxHealth() const;
    float getMoveSpeed() const;
    ActionState getActionState() const;

    void setMovementBlocked(bool blocked);
    bool isMovementBlocked() const;

    virtual void jump();
    virtual void duck();
    bool isJumping() const;
    bool isDucking() const;

    virtual PlayerType getPlayerType() const = 0;
    virtual void startAttack(GameManager& gameManager) = 0;
    virtual bool canAttack() const = 0;

    void updateMovement(float deltaTime);
    void updateJump(float deltaTime);
    void updateDuck(float deltaTime);
    void updateDamageFeedback(float deltaTime);
    void update(float deltaTime) override ;
    void setActionState(ActionState newState);
    bool canMove() const;
};


#endif // PLAYER_H