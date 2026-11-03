#include "CombatSystem.h"
#include "../entities/DefensePlayer.h"
#include "../utils/WorldLayout.h"
#include <algorithm>

CombatSystem::CombatSystem(GameManager* playerManager, CollisionSystem* collisionSystem){
    (*this).playerManager = playerManager;
    (*this).collisionSystem = collisionSystem;
}

void CombatSystem::update(float deltaTime, const std::vector<Enemy*>& activeEnemies){
    setEnemies(activeEnemies); // Update the list of active enemies
 
    std::vector<Player*> activePlayers = (*playerManager).getActivePlayers();
    for (int i=0 ;i< activePlayers.size();i++){
        Player* player = activePlayers[i];
        if (player!=nullptr && (*player).isAlive()){
            player->setMovementBlocked(false);

        }
    }
    for (int i=0;i<activeEnemies.size();i++){
        Enemy* enemy = activeEnemies[i];
        if (enemy!=nullptr && (*enemy).getHealth() > 0){
            enemy->setMovementBlocked(false);
        }
    }

    resolvePlayerAttacks(activeEnemies);
    resolveEnemyAttacks(activeEnemies);
    resolvePlayerProjectiles(activeEnemies);
}

void CombatSystem::setEnemies(const std::vector<Enemy*>& activeEnemies){
    enemies = activeEnemies;
}

void CombatSystem::resolvePlayerAttacks(const std::vector<Enemy*>& activeEnemies){
    std::vector<Player*> activePlayers = (*playerManager).getActivePlayers();

    for (int i=0 ; i<activePlayers.size();i++){
        Player* player= activePlayers[i];
        if (player!=nullptr && player->isAlive()&& (*player).canAttack() && player->getPlayerType()== PlayerType::Melee){
            for(int j=0;j<activeEnemies.size();j++){
                Enemy* enemy = activeEnemies[j];
                if (enemy!=nullptr && (*enemy).getHealth() > 0 &&(*collisionSystem).playerEnemyEncounter(*player, *enemy)){
                    resolvePlayerAttack(*player, *enemy);
                    break;}

                
            }
           
        
            
        }
         if (player!=nullptr && player->isAlive()&& (*player).canAttack() && player->getPlayerType()== PlayerType::Ranged){
                player->startAttack(*playerManager);

        }
    }
}

void CombatSystem::resolvePlayerProjectiles(const std::vector<Enemy*>& activeEnemies)
{
   std::vector<Projectile*> activeprojectile=(*playerManager).getActiveProjectiles();
    for (int i=0;i<activeprojectile.size();i++){
        Projectile* projectile = activeprojectile[i];
        for(int j=0;j<activeEnemies.size();j++){
            Enemy* enemy = activeEnemies[j];
            if (projectile!=nullptr && enemy!=nullptr && (*enemy).getHealth() > 0 && (*projectile).getOwner()!=nullptr && (*collisionSystem).projectileHitsTarget(*projectile, *enemy)){
                enemy->takeDamage(2);
                (*projectile).markHit();
                break;
            }
        }
    }
}

void CombatSystem::resolveEnemyProjectiles(const std::vector<EnemyProjectile*>& shots)
{
    std::vector<Player*> activePlayers = (*playerManager).getActivePlayers();
    for(int i=0 ;i<shots.size();i++){
        EnemyProjectile* shot = shots[i];
        for(int j=0;j<activePlayers.size();j++){
            Player* player = activePlayers[j];
            if (shot!=nullptr && shot->isActive() && player!=nullptr && (*player).isAlive() && (*collisionSystem).intersects(*shot, *player)){
                if((shot->getAttackType() == AttackType::High && !( player->isDucking()))||(shot->getAttackType() == AttackType::Low && !(player->isJumping()))){
                player->takeDamage(shot->getDamage());
                shot->setActive(false);
                break;
            }
        }
    }
}
}

void CombatSystem::resolvePlayerAttack(Player& player, Enemy& enemy){
    player.startAttack(*playerManager);
    enemy.takeDamage(1);
}

void CombatSystem::resolveEnemyAttack(Enemy& enemy, Player& player)
{
    if (enemy.canAttack() && !isPlayerProtected(player, enemies)){
        player.takeDamage(enemy.getDamage());
        enemy.resetAttackTimer();
    }
}

bool CombatSystem::isPlayerProtected(
    const Player& player,
    const std::vector<Enemy*>&) const
{
    if (playerManager == nullptr) {
        return false;
    }

    const std::vector<Player*> activePlayers = playerManager->getActivePlayers();
    return std::any_of(activePlayers.begin(), activePlayers.end(),
        [&player](const Player* candidate) {
            const auto* defensePlayer =
                dynamic_cast<const DefensePlayer*>(candidate);
            return defensePlayer != nullptr && defensePlayer->protects(player);
        });
}

void CombatSystem::resolveEnemyAttacks(const std::vector<Enemy*>& activeEnemies){
    std::vector<Player*> activePlayers = (*playerManager).getActivePlayers();

    for (int i=0;i<activeEnemies.size();i++){
        Enemy* enemy = activeEnemies[i];
        if (enemy !=nullptr && (*enemy).getHealth() > 0){
            AttackType type = (*enemy).getAttackType();
            for(int j=0;j<activePlayers.size();j++){
                Player* player = activePlayers[j];
                if (player!=nullptr && (*player).isAlive()&& (*player).getLaneIndex() == (*enemy).getLane()){
                    float distance= (*enemy).getPosition().x - (*player).getX();
                    if (type == AttackType::Melee)
                    {
                        if (collisionSystem->playerEnemyEncounter(*player, *enemy))
                        {
                            (*enemy).setMovementBlocked(true);
                            (*player).setMovementBlocked(true);

                            if ((*enemy).canAttack())
                            {
                                resolveEnemyAttack(*enemy, *player);
                                
                            }

                            break;
                        }
                    }   
                    
                    else if (type == AttackType::High)
                    {
                        if (!(*player).isDucking())
                        {
                            resolveEnemyAttack(*enemy, *player);
                            break;
                        }
                       
                    }

                    else if (type== AttackType::Low){
                        if(!(*player).isJumping()){
                            resolveEnemyAttack(*enemy, *player);
                            break;
                        }
                    }
                }
            }
        }
    }

    
}