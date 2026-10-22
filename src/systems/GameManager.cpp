#include "../entities/Player.h"
#include "../entities/DefensePlayer.h"
#include "../entities/MeleePlayer.h"
#include "../entities/RangedPlayer.h"
#include "../utils/Types.h"
#include "../utils/Animation.h"
#include "../entities/Entity.h"
#include "../entities/Projectile.h"
#include "../utils/Constants.h"
#include "../world/Grid.h"
#include <vector>
#include <memory>

class GameManager {
    std::vector<std::unique_ptr<Player>> players;
    std::vector<std::unique_ptr<Projectile>> projectiles;
    Grid* grid;

    Player* placePlayer(PlayerType type, int laneIndex) {
        if (type == PlayerType::Defense) {
            return new DefensePlayer(Constants::homebasex, Constants::homebasey, Constants::defensewidth, Constants::defenseheight, laneIndex, Constants::MaxHealthDefense, Constants::speedDefense);
        }
        if (type == PlayerType::Melee) {
            return new MeleePlayer(Constants::homebasex, Constants::homebasey, Constants::meleewidth, Constants::meleeheight, laneIndex, Constants::MaxHealthMelee, Constants::speedMelee);
        }
        if (type == PlayerType::Ranged) {
            return new RangedPlayer(Constants::homebasex, Constants::homebasey, Constants::rangedwidth, Constants::rangedheight, laneIndex, Constants::MaxHealthRanged, Constants::speedRanged);
        }
        return nullptr;
    }

    void update(float deltaTime){
        for (const auto& player : players) {
            player->update(deltaTime);
        }
        for (const auto& projectile : projectiles) {
            projectile->update(deltaTime);
        }
    }

    void addProjectile(std::unique_ptr<Projectile> projectile){
        projectiles.push_back(projectile);
    }

    void removeDefeatedPlayers(){
        players.erase(
        std::remove_if(
            players.begin(),
            players.end(),
            [](const std::unique_ptr<Player>& player) {
                return !player->isAlive();
            }
            ),
            players.end()
        );
    }

    void removeInactiveProjectiles(){
        projectiles.erase(
            std::remove_if(
                projectiles.begin(),
                projectiles.end(),
                [](const std::unique_ptr<Projectile>& projectile) {
                    return !projectile->isActive();
                }
            ),
            projectiles.end()
        );
    }

    std::vector<Player*> getPlayersInLane(int laneIndex) const{
        std::vector<Player*> lanePlayers;
        for (const auto& player : players) {
            if (player->getLaneIndex() == laneIndex) {
                lanePlayers.push_back(player.get());
            }
        }
        return lanePlayers;
    }

    std::vector<Player*> getActivePlayers() const{
        std::vector<Player*> activePlayers;
        for (const auto& player : players) {
            if (player->isAlive()) {
                activePlayers.push_back(player.get());
            }
        }
        return activePlayers;
    }

    std::vector<Projectile*> getActiveProjectiles() const
    {
        std::vector<Projectile*> activeProjectiles;
        for (const auto& projectile : projectiles) {
            if (projectile->isActive()) {
                activeProjectiles.push_back(projectile.get());
            }
        }
        return activeProjectiles;
    }

    Player* getPlayerAtIndex(std::size_t index) const{
        if (index < players.size()) {
            return players[index].get();
        }
        return nullptr;
    }

    void clear(){
        players.clear();
        projectiles.clear();
    }

    void updatePlayerPositions(float deltaTime){
        for (const auto& player : players) {
            player->updateMovement(deltaTime);
        }
    }

    void updateProjectiles(float deltaTime){
        for (const auto& projectile : projectiles) {
            projectile->update(deltaTime);
        }
    }

    void cleanupInactiveEntities(){
        removeDefeatedPlayers();
        removeInactiveProjectiles();
    }
    // void CreateProjectile (Projectile* projectile){
    //     addProjectile(std::unique_ptr<Projectile>(projectile));
    // }
};