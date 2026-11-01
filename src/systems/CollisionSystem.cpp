#include "CollisionSystem.h"
#include "../entities/Enemy.h"
#include "../entities/Player.h"

    bool CollisionSystem::intersects(const Rect& a, const Rect& b) const{
        return (a.x < b.x + b.width) && (a.x + a.width > b.x) &&
               (a.y < b.y + b.height) && (a.y + a.height > b.y);
    }

    bool CollisionSystem::intersects( Entity& a,  Entity& b) const{
        return intersects(a.getBounds(), b.getBounds());
    }

    bool CollisionSystem::projectileHitsTarget( Projectile& projectile,  Entity& target) const{
        return intersects(projectile.getBounds(), target.getBounds());
    }
    
    bool CollisionSystem::sameLane( Entity& a,  Entity& b) const{
        return a.getLaneIndex() == b.getLaneIndex();
    }

    bool CollisionSystem::playerEnemyEncounter( Player& player,  Enemy& enemy) const{ //waiting for enemy to be created to fix this syntax error
        if ((enemy.getPosition().x - player.getX()) <= player.attackRange && player.getLaneIndex() == enemy.getLane()) {
            player.setMovementBlocked(true);
            return true;
        }
        return false; //if they are on the same lane they begin fighting 
    }
