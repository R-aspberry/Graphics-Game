#include "../utils/Types.h"
#include "../utils/Animation.h"
#include "../entities/Entity.h"
#include <Constants.h>
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
            return actionState == ActionState::Moving;
        }

        void MeleePlayer::startAttack(GameManager &gameManager ) {
            if(!canAttack()){
                return;
            }
            actionState = ActionState::Attacking;
            attackTimer = 0.0f;
            

        }

        void MeleePlayer::finishAttack(){
            if(actionState == ActionState::Attacking){
                actionState = ActionState::Moving;
                attackTimer = 0.0f;
                attackCoolDownTimer = GameConfig::MELEE_ATTACK_INTERVAL;
            }
        }

        int MeleePlayer::getAttackDamage () const{
            return attackDamage;
        }

        float MeleePlayer::getAttackRange() const{
            return attackRange;
        }

        bool MeleePlayer::isAttacking () const{
            return ActionState::Attacking == actionState;
        }

        void MeleePlayer::updateAttack(float deltaTime){
            if(actionState != ActionState::Attacking){
                return;
            }
            attackTimer +=deltaTime;
            if (attackTimer >= Constants::attackDuration){
                finishAttack();
            }
            
        }

        void MeleePlayer::update(float deltaTime){
            Player::update(deltaTime);
            this->updateAttack(deltaTime);
            if (attackCoolDownTimer > 0){
                attackCoolDownTimer-= deltaTime;
            }
            if (attackCoolDownTimer < 0){
                attackCoolDownTimer = 0;
            }
        }
