#include "../utils/Types.h"
#include "../utils/Animation.h"
#include "../entities/Entity.h"
#include <Constants.h>
#include "DefensePlayer.h"

    DefensePlayer::DefensePlayer(float x, float y, float width, float height, int laneIndex,  int maxHealth,float moveSpeed) : Player (x,y,width,height,laneIndex,maxHealth,moveSpeed), protectionEnabled(true){

    }

    DefensePlayer::~DefensePlayer() = default;
 
    void DefensePlayer::update(float deltaTime) {
        updateDamageFeedback(deltaTime);
    }

    PlayerType DefensePlayer::getPlayerType() const {
        return PlayerType::Defense;
    }
    void DefensePlayer::startAttack(GameManager &gameManager ) {
        //nothing
    }

    bool DefensePlayer::canAttack() const {
        return false;
    }

    void DefensePlayer::jump() {

    }
    void DefensePlayer::duck() {

    }
    bool DefensePlayer::canProvideProtection() const{
        return ActionState::Defeated != actionState && protectionEnabled;
    }

    void DefensePlayer::disableProtection(){
        protectionEnabled = false;
    }

    bool DefensePlayer::protects(const Player& player) const{
        if (!canProvideProtection() || !(player.isAlive())){
            return false;
        }
        bool cond1 = this->laneIndex == player.laneIndex;
        bool cond2 = this -> x > player.x;
        return cond1&&cond2;

    }

