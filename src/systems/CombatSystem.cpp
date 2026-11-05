#include "CombatSystem.h"
#include "../entities/DefensePlayer.h"
#include "../utils/Constants.h"
#include "../systems/WaveManager.h"
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

    updateEncounters(activeEnemies);
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
                enemy->takeDamage(GameConfig::RANGED_PLAYER_DAMAGE);
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

            if (shot!=nullptr && shot->isActive() && player!=nullptr && (*player).isAlive() && (*collisionSystem).intersects(*shot, *player) && (*player).getLaneIndex() == (*shot).getLaneIndex())
              { DefensePlayer* defense = dynamic_cast<DefensePlayer*>(player);
                if ((shot->getAttackType() == AttackType::High && player->isDucking()) ||
                    (shot->getAttackType() == AttackType::Low && player->isJumping()))
                {
                    continue;
                }
                Player* target = player;

                            for (Player* candidate : activePlayers)
                            {
                                DefensePlayer* defense = dynamic_cast<DefensePlayer*>(candidate);

                                if (defense != nullptr && defense->protects(*player))
                                {
                                    target = defense;
                                    break;
                                }
                            }

                            target->takeDamage(shot->getDamage());
                            shot->setActive(false);
                            break;

                    }
             
            }
        
    }
}


void CombatSystem::resolvePlayerAttack(Player& player, Enemy& enemy){
    player.startAttack(*playerManager);
    enemy.takeDamage(GameConfig::MELEE_PLAYER_DAMAGE);
}

void CombatSystem::resolveEnemyAttack(Enemy& enemy, Player& player)
{
    if (!enemy.canAttack())
        return;

    Player* target = &player;

    for (Player* candidate : playerManager->getActivePlayers())
    {
        DefensePlayer* defense = dynamic_cast<DefensePlayer*>(candidate);

        if (defense != nullptr && defense->protects(player))
        {
            target = defense;
            break;
        }
    }

    target->takeDamage(enemy.getDamage());
    enemy.resetAttackTimer();

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

void CombatSystem::updateEncounters(const std::vector<Enemy*>& activeEnemies){
    std::vector<Player*> activePlayers = (*playerManager).getActivePlayers();
    for(int i=0;i< activePlayers.size();i++){
        Player* player = activePlayers[i];
        for(int j=0 ;j<activeEnemies.size();j++){
            Enemy* enemy= activeEnemies[j];
            if(player!=nullptr && enemy!=nullptr && player->isAlive() && enemy->getHealth() > 0 && (*collisionSystem).playerEnemyEncounter(*player, *enemy)){
                // Every colliding pair stops, including ranged enemies and
                // players whose attack is still on cooldown.
                player->setMovementBlocked(true);
                enemy->setMovementBlocked(true);
            }
        }
    }
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
                    
                    
                }
            }
        }
    }

    
}


