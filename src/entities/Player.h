#define PLAYER_H

#include "Entity.h"
#include "../utils/Types.h"
#include "../utils/Animation.h"

class Player : public Entity {
public:
    int health;
    int maxHealth;
    float moveSpeed;
    ActionState actionState;
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
    virtual void startAttack() = 0;
    virtual bool canAttack() const = 0;

    void updateMovement(float deltaTime);
    void updateJump(float deltaTime);
    void updateDuck(float deltaTime);
    void updateDamageFeedback(float deltaTime);
    void setActionState(ActionState newState);
};


