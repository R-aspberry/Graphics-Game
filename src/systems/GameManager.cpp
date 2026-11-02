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
#include <utility>

#include "GameManager.h"

GameManager::GameManager() : grid(nullptr) {}

GameManager::~GameManager() = default;

void GameManager::setGrid(Grid* gameGrid) {
    grid = gameGrid;
}

    Player* GameManager::placePlayer(PlayerType type, int laneIndex) {
        const Lane& lane = grid->getLanes()[laneIndex];

        float playerX = (lane.left + 0.025f) * 1000.0f;
        float laneCenterY = (lane.top + lane.bottom) / 2.0f;
        float playerY = (1.0f - laneCenterY - 0.05f) * 1000.0f;
        std::unique_ptr<Player> player = nullptr;
        if (type == PlayerType::Defense) {
            player =  std::make_unique<DefensePlayer>(playerX, playerY, Constants::defensewidth, Constants::defenseheight, laneIndex, Constants::MaxHealthDefense, Constants::speedDefense);
        }
        if (type == PlayerType::Melee) {
            player =  std::make_unique<MeleePlayer>(playerX, playerY, Constants::meleewidth, Constants::meleeheight, laneIndex, Constants::MaxHealthMelee, Constants::speedMelee);
            player->setActionState(ActionState::Moving);
        }
        if (type == PlayerType::Ranged) {
            player =  std::make_unique<RangedPlayer>(playerX, playerY, Constants::rangedwidth, Constants::rangedheight, laneIndex, Constants::MaxHealthRanged, Constants::speedRanged);
            player->setActionState(ActionState::Moving);
        }
        
        if(player == nullptr){
            return nullptr;
        }
        Player* out = player.get();
        players.push_back(std::move(player));
        return out;
    }

    void GameManager::update(float deltaTime){
        for (const auto& player : players) {
            player->update(deltaTime);
        }
        for (const auto& projectile : projectiles) {
            projectile->update(deltaTime);
        }
    }

    void GameManager::addProjectile(std::unique_ptr<Projectile> projectile){
        projectiles.push_back(std::move(projectile));
    }

    void GameManager::removeDefeatedPlayers(){
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

    void GameManager::removeInactiveProjectiles(){
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

    std::vector<Player*> GameManager::getPlayersInLane(int laneIndex) const{
        std::vector<Player*> lanePlayers;
        for (const auto& player : players) {
            if (player->getLaneIndex() == laneIndex) {
                lanePlayers.push_back(player.get());
            }
        }
        return lanePlayers;
    }

    std::vector<Player*> GameManager::getActivePlayers() const{
        std::vector<Player*> activePlayers;
        for (const auto& player : players) {
            if (player->isAlive() && player->isActive()) {
                activePlayers.push_back(player.get());
            }
        }
        return activePlayers;
    }

    std::vector<Projectile*> GameManager::getActiveProjectiles() const
    {
        std::vector<Projectile*> activeProjectiles;
        for (const auto& projectile : projectiles) {
            if (projectile->isActive()) {
                activeProjectiles.push_back(projectile.get());
            }
        }
        return activeProjectiles;
    }

    Player* GameManager::getPlayerAtIndex(std::size_t index) const{
        if (index < players.size()) {
            return players[index].get();
        }
        return nullptr;
    }

    void GameManager::clear(){
        players.clear();
        projectiles.clear();
    }

    void GameManager::updatePlayerPositions(float deltaTime){
        for (const auto& player : players) {
            player->updateMovement(deltaTime);
        }
    }

    void GameManager::updateProjectiles(float deltaTime){
        for (const auto& projectile : projectiles) {
            projectile->update(deltaTime);
        }
    }

    void GameManager::cleanupInactiveEntities(){
        removeDefeatedPlayers();
        removeInactiveProjectiles();
    }


    
