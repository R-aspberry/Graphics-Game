#include "../utils/Types.h"
#include "../utils/Animation.h"
#include "../entities/Entity.h"
#include "../entities/Projectile.h"
#include <Constants.h>
#include <algorithm>
#include <cmath>
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
        return active && isAlive() && !attacking;
    }

    void RangedPlayer::startAttack(GameManager& gameManager) {
        if(!canAttack()){
            return;
        }
        attacking = true;
            if (!isJumping() && !isDucking()) actionState = ActionState::Attacking;
            animation.reset();
        attackTimer = 0.0f;
            attackCoolDownTimer = GameConfig::RANGED_ATTACK_INTERVAL;
        // Start at the weapon/body, so an overlapping enemy at the entry edge
        // cannot end up behind every shot's collision box.
        auto projectile  = std::make_unique<Projectile>( x + width / 2.0f, y + height / 2.0f - Constants::ProjectileHeight / 2.0f,Constants::ProjectileWidth, Constants::ProjectileHeight,laneIndex ,Constants::ProjectileSpeed,this );
        gameManager.addProjectile(std::move(projectile));
    }

    void RangedPlayer::finishAttack(){
            attacking = false;
            attackTimer = 0.0f;
            if (actionState == ActionState::Attacking) actionState = ActionState::Moving;
        }

        int RangedPlayer::getAttackDamage() const{
        return attackDamage;
    }

    float RangedPlayer::getAttackRange() const{
        return attackRange;
    }

    bool RangedPlayer::isAttacking () const{
        return attacking;
    }

    void RangedPlayer::updateAttack(float deltaTime){
        if(!attacking){
            return;
        }
        attackTimer +=deltaTime;
        if (attackTimer >= Constants::attackDuration){
            finishAttack();
        }
        
    }

    void RangedPlayer::update(float deltaTime){
            if (!std::isfinite(deltaTime) || deltaTime < 0 || !isAlive()) return;
            attackCoolDownTimer = std::max(0.0f, attackCoolDownTimer - deltaTime);
            Player::update(deltaTime);
            updateAttack(deltaTime);
        }
