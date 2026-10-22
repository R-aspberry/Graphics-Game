#include "../utils/Types.h"
#include "../utils/Animation.h"
#include "../entities/Entity.h"
#include <Constants.h>
#include "Player.h"

class DefensePlayer : public Player{
    bool protectionEnabled;

    DefensePlayer(float x, float y, float width, float height, int laneIndex, float moveSpeed, int maxHealth) : Player (x,y,width,height,laneIndex,maxHealth,moveSpeed){

    }

    void update(float deltaTime) override{
        updateDamageFeedback(deltaTime);
    }

    PlayerType getPlayerType() const override{
        return PlayerType::Defense;
    }
    void startAttack() override{
        //nothing
    }

    bool canAttack() const override{
        return false;
    }

    void jump() override{

    }
    void duck() override{

    }
    bool canProvideProtection() const{
        return ActionState::Defeated != actionState && protectionEnabled;
    }

    void disableProtection(){
        protectionEnabled = false;
    }

    bool protects(const Player& player) const{
        if (!canProvideProtection || !(player.isAlive())){
            return ;
        }
        bool cond1 = this->laneIndex == player.laneIndex;
        bool cond2 = this -> x > player.x;
        return cond1&cond2;

    }
};

