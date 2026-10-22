#include "Player.h"
class Projectile : public Entity {
    float speed;
    float x ;
    float y;
    Player* player;
    bool hasHit;

    Projectile(float speed, float x ,float y, Player* player, int laneIndex): Entity(x,y,0,0,laneIndex){
        this->speed = speed;
        this-> player = player;
        hasHit = false;
    }
    void update(float deltaTime) override{
        x += speed*deltaTime;
    }

    Player* getOwner() const{
        return this->player;
    }
    bool hasAlreadyHit() const{
        return hasHit;
    }
    void markHit(){
        hasHit = true;
    }
    float getSpeed(){
        return speed;
    }

    float getX(){
        return x;
    }
    float getY(){
        return y;
    }
};