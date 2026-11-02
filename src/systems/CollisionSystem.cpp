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

    bool CollisionSystem::playerEnemyEncounter( Player& player,  Enemy& enemy) const{ 
       if(player.getLaneIndex()!= enemy.getLane()){
        return false;
       }
       float playerx = player.getX()+ player.getWidth();
       float enemyx = enemy.getPosition().x - 37.5f; //why the heck 37.5f we have no clue 

       if(playerx>=enemyx){
        player.setMovementBlocked(true);
        enemy.setMovementBlocked(true);
        return true;
       }
       return false;
    }

    