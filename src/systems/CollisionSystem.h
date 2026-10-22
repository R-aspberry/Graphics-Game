#define CollisionSystem_H

#include <Types.h>
#include <Entity.h>
#include <Projectile.h>
class CollisionSystem{
    bool intersects(const Rect& a, const Rect& b) const;

    bool intersects( Entity& a,  Entity& b) const;
    

    bool projectileHitsTarget( Projectile& projectile,  Entity& target) const ;
    
    bool sameLane( Entity& a,  Entity& b) const;

    bool playerEnemyEncounter( Player& player,  Enemy& enemy) const; //waiting for enemy to be created to fix this syntax error
       //if they are on the same lane they begin fighting 
    
};