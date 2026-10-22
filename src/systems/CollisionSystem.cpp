#include <Types.h>
#include <Entity.h>
#include <Projectile.h>
class CollisionSystem{
    bool intersects(const Rect& a, const Rect& b) const{
        return (a.x < b.x + b.width) && (a.x + a.width > b.x) &&
               (a.y < b.y + b.height) && (a.y + a.height > b.y);
    }

    bool intersects( Entity& a,  Entity& b) const{
        return intersects(a.getBounds(), b.getBounds());
    }

    bool projectileHitsTarget( Projectile& projectile,  Entity& target) const{
        return intersects(projectile.getBounds(), target.getBounds());
    }
    
    bool sameLane( Entity& a,  Entity& b) const{
        return a.getLaneIndex() == b.getLaneIndex();
    }

    bool playerEnemyEncounter( Player& player,  Enemy& enemy) const{ //waiting for enemy to be created to fix this syntax error
        return (enemy.getX - player.getX) <= player.attackRange; //if they are on the same lane they begin fighting 
    }
};