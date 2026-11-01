#include "CombatSystem.h"

CombatSystem::CombatSystem(GameManager* playerManager, CollisionSystem* collisionSystem){
    (*this).playerManager = playerManager;
    (*this).collisionSystem = collisionSystem;
}

void CombatSystem::update(float deltaTime, const std::vector<Enemy*>& activeEnemies){
    setEnemies(activeEnemies); // Update the list of active enemies
    resolvePlayerAttacks(activeEnemies);
    resolveEnemyAttacks(activeEnemies);
   

}

void CombatSystem::setEnemies(const std::vector<Enemy*>& activeEnemies){
    enemies = activeEnemies;
}

void CombatSystem::resolvePlayerAttacks(const std::vector<Enemy*>& activeEnemies){
    std::vector<Player*> activePlayers = (*playerManager).getActivePlayers();

    for (int i=0 ; i<activePlayers.size();i++){
        Player* player= activePlayers[i];
        if ((*player).canAttack()){
            for(int j=0;j<activeEnemies.size();j++){
                Enemy* enemy = activeEnemies[j];
                if (enemy!=nullptr && (*enemy).getHealth() > 0 &&(*collisionSystem).playerEnemyEncounter(*player, *enemy)){
                    resolvePlayerAttack(*player, *enemy);
                    break;}
            }

        }
    }
}

void CombatSystem::resolvePlayerAttack(Player& player, Enemy& enemy){
    player.startAttack(*playerManager);
}

void CombatSystem::resolveEnemyAttack(Enemy& enemy, Player& player)
{
    if (enemy.canAttack() && !isPlayerProtected(player, enemies)){
        player.takeDamage(enemy.getDamage());
        enemy.resetAttackTimer();
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
                        if (distance >= 0 && distance <= (*player).getWidth())
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

