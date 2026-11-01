#include "../utils/Types.h"
#include "../utils/Animation.h"
#include "../entities/Entity.h"
#include "../entities/Projectile.h"
#include <Constants.h>
#include "RangedPlayer.h"
#include "GameManager.h"



    RangedPlayer::RangedPlayer(float x, float y, float width, float height, int laneIndex, int maxHealth, float moveSpeed) : Player (x,y,width,height,laneIndex,maxHealth,moveSpeed){
        

    }

    RangedPlayer::~RangedPlayer() = default;

    PlayerType RangedPlayer::getPlayerType() const {
        return PlayerType::Ranged;
    }

    bool RangedPlayer::canAttack() const {
        if (!(attackCoolDownTimer<=0.0f)){
            return false;
        }
        return actionState == ActionState::Moving;
    }

    void RangedPlayer::startAttack(GameManager& gameManager) {
        if(!canAttack()){
            return;
        }
        actionState = ActionState::Attacking;
        attackTimer = 0.0f;
        auto projectile  = std::make_unique<Projectile>( x + width, y + height / 2.0f,Constants::ProjectileWidth, Constants::ProjectileHeight,laneIndex ,Constants::ProjectileSpeed,this );
        gameManager.addProjectile(std::move(projectile));
    }

    void RangedPlayer::finishAttack(){
        if(actionState == ActionState::Attacking){
            actionState = ActionState::Moving;
            attackTimer = 0.0f;
            attackCoolDownTimer = Constants::attackIntervals;
        }
    }

    int RangedPlayer::getAttackDamage() const{
        return attackDamage;
    }

    float RangedPlayer::getAttackRange() const{
        return attackRange;
    }

    bool RangedPlayer::isAttacking () const{
        return ActionState::Attacking == actionState;
    }

    void RangedPlayer::updateAttack(float deltaTime){
        if(actionState != ActionState::Attacking){
            return;
        }
        attackTimer +=deltaTime;
        if (attackTimer >= Constants::attackDuration){
            finishAttack();
        }
        
    }

    void RangedPlayer::update(float deltaTime){
        this->updateAttack(deltaTime);
        Player::update(deltaTime);
        attackCoolDownTimer-= deltaTime;
    }
