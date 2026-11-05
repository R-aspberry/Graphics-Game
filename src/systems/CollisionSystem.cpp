#include "CollisionSystem.h"
#include "../entities/Enemy.h"
#include "../entities/Player.h"
#include "../utils/Constants.h"
#include "../entities/Collectible.h"

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
       Rect enemyBounds = {
            enemy.getPosition().x,
            enemy.getPosition().y,
            WorldLayout::ENEMY_WIDTH,
            WorldLayout::ENEMY_HEIGHT
        };

        Rect playerBounds = player.getBounds();

       if(intersects(playerBounds, enemyBounds)){
            player.setMovementBlocked(true);
            enemy.setMovementBlocked(true);
            return true;
       }
       return false;

    }

    bool CollisionSystem::projectileHitsTarget(Projectile& projectile, Enemy& enemy) const
    {
        if (projectile.getLaneIndex() == enemy.getLane())
        {
            if (
                projectile.getX() + projectile.getWidth() > enemy.getPosition().x &&
                projectile.getX() < enemy.getPosition().x + WorldLayout::ENEMY_WIDTH &&
                projectile.getY() + projectile.getHeight() > enemy.getPosition().y &&
                projectile.getY() < enemy.getPosition().y + WorldLayout::ENEMY_HEIGHT
            )
            {
                return true;
            }
        }

    

        return false;
    }

    bool CollisionSystem::playerTouchesCollectible( Player& player, Collectible& coin) const {
        if (coin.isCollected())
            return false;

        return sameLane(player, coin) && intersects(player, coin);
    }

    
