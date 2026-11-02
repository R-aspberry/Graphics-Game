#include "../utils/Types.h"
#include "../utils/Animation.h"
#include "../entities/Entity.h"
#include <Constants.h>
#include <algorithm>
#include <cmath>
#include "MeleePlayer.h"


        MeleePlayer::MeleePlayer(float x, float y, float width, float height, int laneIndex,  int maxHealth, float moveSpeed) : Player (x,y,width,height,laneIndex,maxHealth,moveSpeed){
        }

        MeleePlayer::~MeleePlayer() = default;

        PlayerType MeleePlayer::getPlayerType() const {
            return PlayerType::Melee;
        }

        bool MeleePlayer::canAttack() const {
            if (!(attackCoolDownTimer<=0.0f)){
                return false;
            }
            return active && isAlive() && !attacking;
        }

        void MeleePlayer::startAttack(GameManager &gameManager ) {
            if(!canAttack()){
                return;
            }
            attacking = true;
            if (!isJumping() && !isDucking()) actionState = ActionState::Attacking;
            animation.reset();
            attackTimer = 0.0f;
            attackCoolDownTimer = GameConfig::MELEE_ATTACK_INTERVAL;
            

        }

        void MeleePlayer::finishAttack(){
            attacking = false;
            attackTimer = 0.0f;
            if (actionState == ActionState::Attacking) actionState = ActionState::Moving;
        }

        int MeleePlayer::getAttackDamage () const{
            return attackDamage;
        }

        float MeleePlayer::getAttackRange() const{
            return attackRange;
        }

        bool MeleePlayer::isAttacking () const{
            return attacking;
        }

        void MeleePlayer::updateAttack(float deltaTime){
            if(!attacking){
                return;
            }
            attackTimer +=deltaTime;
            if (attackTimer >= Constants::attackDuration){
                finishAttack();
            }
            
        }

        void MeleePlayer::update(float deltaTime){
            if (!std::isfinite(deltaTime) || deltaTime < 0 || !isAlive()) return;
            attackCoolDownTimer = std::max(0.0f, attackCoolDownTimer - deltaTime);
            Player::update(deltaTime);
            updateAttack(deltaTime);
        }
