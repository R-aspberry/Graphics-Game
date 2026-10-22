#include "../utils/Types.h"
#include "../utils/Animation.h"
#include "../entities/Entity.h"
#include <Constants.h>

class Player : public Entity{
    int health;
    int maxHealth;
    float moveSpeed;
    ActionState actionState;
    bool movementBlocked;
    float jumpTimer;
    float duckTimer;
    float damageFlashTimer;
    float attackRange;
    Animation animation; // error will go away once we define animation class

    Player(float x, float y, float width, float height, int laneIndex, int maxHealth, float moveSpeed) : Entity(x, y, width, height, laneIndex){
        this->maxHealth = maxHealth;
        this-> moveSpeed = moveSpeed;
    }

    ~Player() override {

    }

    virtual void takeDamage(int damage){

    }

    bool isAlive() {
        return actionState != ActionState::Defeated;
    }
    int getHealth() const {
        return health;
    }
    int getMaxHealth() const{
        return maxHealth;
    }
    float getMoveSpeed() const	{
        return moveSpeed;
    }
    ActionState getActionState() const	{
        return actionState;
    }
    void setMovementBlocked(bool blocked)	{
        movementBlocked = true;
    }
    bool isMovementBlocked() const	{
        return movementBlocked;
    }
    virtual void jump()	{
        
    }
    virtual void duck()	{
        
    }
    bool isJumping() const	{
        return actionState == ActionState::Jumping;
    }
    bool isDucking() const	{
            return actionState == ActionState::Ducking;
    }
    virtual PlayerType getPlayerType() const = 0 ;	
    virtual void startAttack() = 0	;
    virtual bool canAttack() const = 0 ;

    void updateMovement(float deltaTime) {
        if(ActionState::Moving == actionState && movementBlocked== false){
            x+= moveSpeed * deltaTime;
        }
    }

    void updateJump (float deltaTime){
        if (actionState != ActionState::Jumping){
            return;
        }
        jumpTimer += deltaTime;
        if(jumpTimer > Constants::JUMP_DURATION){
            jumpTimer = 0.0f;
            if(movementBlocked){
                actionState = ActionState::Idle;
            }   
            else {
                actionState = ActionState::Moving;
            }
        }
    }

    void updateDuck (float deltaTime){
        if (actionState != ActionState::Ducking){
            return;
        }
        duckTimer += deltaTime;
        if(duckTimer > Constants::DUCK_DURATION){
            duckTimer = 0.0f;
            if(movementBlocked){
                actionState = ActionState::Idle;
            }   
            else {
                actionState = ActionState::Moving;
            }
        }
    }

    void Player::updateDamageFeedback(float deltaTime)
    {
        if (damageFlashTimer <= 0.0f)
        {
            return;
        }
        damageFlashTimer -= deltaTime;

        if (damageFlashTimer < 0.0f)
        { damageFlashTimer = 0.0f;
        }
    }
    void update(float deltaTime) override {
        updateMovement(deltaTime);
        updateJump(deltaTime);
        updateDuck(deltaTime);
        updateDamageFeedback(deltaTime);
    }

    void setActionState(ActionState newState){
        actionState = newState;
    }
    virtual bool canMove(){
        return !movementBlocked;
    }
};