#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "Entity.h"
#include "Player.h"


class Projectile : public Entity {
private:
    float speed;
    // float x;
    // float y;
    Player* player;
    bool hasHit;

public:
    Projectile(float x , float y , float width , float height, int laneIndex ,  float speed,  Player* player);
    ~Projectile() override;

    void update(float deltaTime) override;

    Player* getOwner() const;
    bool hasAlreadyHit() const;
    void markHit() ;

    float getSpeed() const;
};

#endif // PROJECTILE_H
