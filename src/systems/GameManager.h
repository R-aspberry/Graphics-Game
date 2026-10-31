#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <vector>
#include <memory>
#include "../entities/Player.h"
#include "../entities/Projectile.h"
#include "../world/Grid.h"
#include "../utils/Types.h"

class GameManager {
public:
    std::vector<std::unique_ptr<Player>> players;
    std::vector<std::unique_ptr<Projectile>> projectiles;
    Grid* grid;

public:
    GameManager();
    virtual ~GameManager();

    // Core Game Loop
    void update(float deltaTime);

    // Player and Projectile Management
    Player* placePlayer(PlayerType type, int laneIndex);
    void addProjectile(std::unique_ptr<Projectile> projectile);
    void removeDefeatedPlayers();

    // Accessors
    const std::vector<std::unique_ptr<Player>>& getPlayers() const;
    Grid* getGrid() const { return grid; }

    void removeInactiveProjectiles();
    std::vector<Player*> getPlayersInLane(int laneIndex) const;
    std::vector<Player*> getActivePlayers() const;
    std::vector<Projectile*> getActiveProjectiles() const;
    Player* getPlayerAtIndex(std::size_t index) const;
    void clear();
    void updatePlayerPositions(float deltaTime);
    void updateProjectiles(float deltaTime);
    void cleanupInactiveEntities();
   
};

#endif // GAMEMANAGER_H
