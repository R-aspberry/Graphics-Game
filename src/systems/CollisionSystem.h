#ifndef COLLISION_SYSTEM_H
#define COLLISION_SYSTEM_H

#include "../entities/Entity.h"
#include "../entities/Player.h"
#include "../entities/Projectile.h"
#include "../entities/Enemy.h"

class CollisionSystem {
public:
    bool intersects(const Rect& a, const Rect& b) const;
    bool intersects(Entity& a, Entity& b) const;
    bool projectileHitsTarget(Projectile& projectile, Entity& target) const;
    bool sameLane(Entity& a, Entity& b) const;
    bool playerEnemyEncounter(Player& player, Enemy& enemy) const;
};

#endif