#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "Entity.h"
#include "Player.h"

/**
 * Projectile represents a fired object in the game world.
 * Handles movement and tracks the origin player for scoring/collision.
 */
class Projectile : public Entity {
private:
    float speed;
    float x;
    float y;
    Player* player;
    bool hasHit;

public:
    Projectile(float speed, float x, float y, Player* player, int laneIndex);
    virtual ~Projectile() override;

    void update(float deltaTime) override;

    Player* getOwner() const;
    bool hasAlreadyHit() const;
    void markHit();

    float getSpeed() const;
    float getX() const;
    float getY() const;
};

#endif // PROJECTILE_H
