#include "../utils/Types.h"
#include "../utils/Animation.h"
#include "../entities/Entity.h"
#include <Constants.h>
#include "Player.h"


    Player::Player(float x, float y, float width, float height, int laneIndex, int maxHealth, float moveSpeed) : Entity(x, y, width, height, laneIndex) , health(maxHealth),
      maxHealth(maxHealth),
      moveSpeed(moveSpeed),
      actionState(ActionState::Idle),
      PrevactionState(ActionState::Moving),
      movementBlocked(false),
      jumpTimer(0.0f),
      duckTimer(0.0f),
      damageFlashTimer(0.0f),
      attackRange(0.0f){
    }

    Player::~Player()  {

    }

     void Player::takeDamage(int damage){
        if (actionState == ActionState::Defeated){
            return;
        }
        if (damage<=0){
            return ;
        }
        PrevactionState = actionState;
        health-= damage;
        damageFlashTimer = 0.2f;

        if(health <=0){
            health =0;
            setActive(false);
            actionState = ActionState::Defeated;
        }
        else{
            actionState = ActionState::Damaged;
        }
    }

    bool Player::isAlive() const {
        return actionState != ActionState::Defeated /*|| health!=0*/;
    }
    int Player::getHealth() const {
        return health;
    }
    int Player::getMaxHealth() const{
        return maxHealth;
    }
    float Player::getMoveSpeed() const	{
        return moveSpeed;
    }
    ActionState Player::getActionState() const	{
        return actionState;
    }
    void Player::setMovementBlocked(bool blocked)	{
        movementBlocked = blocked;
        //actionState = ActionState::
    }
    bool Player::isMovementBlocked() const	{
        return movementBlocked;
    }
     void Player::jump()	{
        if (actionState == ActionState::Idle || actionState == ActionState::Moving){
            setActionState(ActionState::Jumping);
            jumpTimer = 0.0f;
        }
        else {
            return;
        }
    }
     void Player::duck()	{
        if (actionState == ActionState::Idle || actionState == ActionState::Moving){
            setActionState(ActionState::Ducking);
            duckTimer = 0.0f;
        }
        else {
            return;
        }
    }
    bool Player::isJumping() const	{
        return actionState == ActionState::Jumping;
    }
    bool Player::isDucking() const	{
            return actionState == ActionState::Ducking;
    }

    void Player::updateMovement(float deltaTime) {
        if(ActionState::Moving == actionState && movementBlocked== false){
            x+= moveSpeed * deltaTime;
        }
    }

    void Player::updateJump (float deltaTime){
        if (actionState != ActionState::Jumping){
            return;
        }
        jumpTimer += deltaTime;
        if(jumpTimer >= Constants::JUMP_DURATION){
            jumpTimer = 0.0f;
            actionState = ActionState::Moving;
        }
    }

    void Player::updateDuck (float deltaTime){
        if (actionState != ActionState::Ducking){
            return;
        }
        duckTimer += deltaTime;
        if(duckTimer >=Constants::DUCK_DURATION){
            duckTimer = 0.0f;
            actionState = ActionState::Moving;
        }
    }

    void Player::updateDamageFeedback(float deltaTime)
    {
        if (damageFlashTimer <= 0.0f)
        {
            return;
        }
        damageFlashTimer -= deltaTime;

        if (damageFlashTimer <= 0.0f)
        { damageFlashTimer = 0.0f;
            if(actionState!= ActionState::Defeated){
            actionState = PrevactionState;
        }
        }

        
    }
    void Player::update(float deltaTime)  {
        updateMovement(deltaTime);
        updateJump(deltaTime);
        updateDuck(deltaTime);
        updateDamageFeedback(deltaTime);
    }

    void Player::setActionState(ActionState newState){
        actionState = newState;
    }
     bool Player::canMove() const{
        return !movementBlocked;
    }
