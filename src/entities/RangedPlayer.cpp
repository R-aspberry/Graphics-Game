#include "../utils/Types.h"
#include "../utils/Animation.h"
#include "../entities/Entity.h"
#include "../entities/Projectile.h"
#include <Constants.h>
#include "Player.h"



class RangedPlayer :public Player{
    int attackDamage = 1 ; // not mentioned 
    //float attackRange = 0.6f; //i still dk ??
    float attackCoolDownTimer;
    float attackTimer;
    //float attackSpeed = 0.3f;
    //bool attacking;

    RangedPlayer(float x, float y, float width, float height, int laneIndex, float moveSpeed, int maxHealth) : Player (x,y,width,height,laneIndex,maxHealth,moveSpeed){
        attackRange = 0.6f;
    }

    PlayerType getPlayerType() const override{
        return PlayerType::Melee;
    }

    bool canAttack() const override{
        if (!(attackCoolDownTimer<=0.0f)){
            return false;
        }
        return actionState == ActionState::Moving;
    }

    void startAttack() override{
        if(!canAttack()){
            return;
        }
        actionState = ActionState::Attacking;
        attackTimer = 0.0f;
        Projectile* projectile  = new Projectile(Constants::ProjectileSpeed, x + width, y + height / 2.0f,this, laneIndex);
        projectiles.push_back(projectile);
    }

    void finishAttack(){
        if(actionState == ActionState::Attacking){
            actionState = ActionState::Moving;
            attackTimer = 0.0f;
            attackCoolDownTimer = Constants::attackIntervals;
        }
    }

    int getAttackDamage() {
        return attackDamage;
    }

    float getAttackRange() const{
        return attackRange;
    }

    bool isAttacking (){
        return ActionState::Attacking == actionState;
    }

    void updateAttack(float deltaTime){
        if(actionState != ActionState::Attacking){
            return;
        }
        attackTimer +=deltaTime;
        if (attackTimer >= Constants::attackDuration){
            finishAttack();
        }
        
    }

    void update(float deltaTime){
        this->updateAttack(deltaTime);
        this->update(deltaTime);
        attackCoolDownTimer-= deltaTime;
    }

};