#include "Game.h"
#include "utils/Constants.h"
#include "entities/RangedPlayer.h"
#include <array>
#include <cmath>
#include <iostream>
#include <limits>

// Play through the real simulation using purchases and selected-lane evasion.
// This test never grants coins, changes health/positions, or kills enemies directly.
int playMatch(unsigned int seed) {
    Game game;
    game.getWaveManager().setRandomSeed(seed);
    const float step = 1.0f / 120.0f;
    for (int tick = 0; tick < 120 * 240 && !game.hasWon() && !game.hasLost(); ++tick) {
        auto& waves = game.getWaveManager();
        std::array<int, 4> threats{}, fighters{};
        std::array<bool, 4> hard{};
        for (std::size_t i = 0; i < waves.getActiveEnemyCount(); ++i) {
            Enemy* enemy = waves.getEnemyAt(i);
            if (enemy->getHealth() <= 0) continue;
            ++threats[enemy->getLane()];
            if (enemy->getEnemyType() == EnemyType::HARD) hard[enemy->getLane()] = true;
        }
        for (Player* player : game.getGameManager().getActivePlayers())
            if (player->getPlayerType() != PlayerType::Defense) ++fighters[player->laneIndex];
        for (int lane = 0; lane < 4; ++lane) {
            if (threats[lane] && fighters[lane] == 0 && game.purchasePlayer(PlayerType::Melee, lane))
                ++fighters[lane];
        }
        for (int lane = 0; lane < 4; ++lane) {
            if (hard[lane] && fighters[lane] < 2 && game.purchasePlayer(PlayerType::Melee, lane))
                ++fighters[lane];
        }
        // Evade the nearest imminent high/low attack in each lane.
        for (int lane = 0; lane < 4; ++lane) {
            AttackType dodge = AttackType::Melee;
            float soonest = .35f;
            for (Player* player : game.getGameManager().getActivePlayers()) {
                if (player->laneIndex != lane || player->getPlayerType() == PlayerType::Defense) continue;
                for (std::size_t i = 0; i < waves.getEnemyProjectilesCount(); ++i) {
                    EnemyProjectile* shot = waves.getEnemyProjectileAt(i);
                    if (!shot->isActive() || shot->laneIndex != lane || shot->x + shot->width < player->x) continue;
                    float seconds = (shot->x - player->x - player->width) /
                        (shot->getSpeed() + (player->isMovementBlocked() ? 0 : player->getMoveSpeed()));
                    if (seconds < soonest) { soonest = seconds; dodge = shot->getAttackType(); }
                }
                for (std::size_t i = 0; i < waves.getActiveEnemyCount(); ++i) {
                    Enemy* enemy = waves.getEnemyAt(i);
                    if (enemy->getHealth() <= 0 || enemy->getLane() != lane ||
                        enemy->getAttackType() == AttackType::Melee) continue;
                    if (std::fabs(enemy->getPosition().x - player->x) < .08f &&
                        enemy->getTimeSinceLastAttack() >= enemy->getAttackInterval() - .08f && soonest > 0) {
                        soonest = 0;
                        dodge = enemy->getAttackType();
                    }
                }
            }
            if (dodge == AttackType::Melee) continue;
            for (int index = 0; index < 4; ++index)
                if (game.getGrid().getLanes()[index].selected != (index == lane)) game.getGrid().toggleLaneSelection(index);
            if (dodge == AttackType::High) game.duckSelectedLanes();
            if (dodge == AttackType::Low) game.jumpSelectedLanes();
        }
        game.update(step);
        if (game.getEconomy().getCoins() < 0) return -1;
        for (Player* player : game.getGameManager().getActivePlayers()) {
            if (player->health <= 0 || player->health > player->maxHealth ||
                player->laneIndex < 0 || player->laneIndex >= 4) return -1;
        }
    }
    std::cout << "seed " << seed << ": " << (game.hasWon() ? "WON" : game.hasLost() ? "LOST" : "TIMEOUT")
              << ", wave " << game.getWaveManager().getCurrentWave()
              << ", coins " << game.getEconomy().getCoins() << '\n';
    if (game.hasWon()) {
        if (game.getWaveManager().getCurrentWave() != 3 ||
            game.getWaveManager().getEnemiesSpawnedInWave() != GameConfig::WAVE_3_COUNT) return -1;
        for (std::size_t i = 0; i < game.getWaveManager().getActiveEnemyCount(); ++i)
            if (game.getWaveManager().getEnemyAt(i)->getHealth() > 0) return -1;
        return 1;
    }
    if (game.hasLost() && game.getWaveManager().hasEnemyReachedBase(WorldLayout::HOME_BASE_X)) return 0;
    return -1; // A stalled match or a result without its required condition is a failure.
}
int main() {
    int wins = 0, losses = 0, failures = 0;
    for (unsigned int seed = 1; seed <= 12; ++seed) {
        int result = playMatch(seed);
        if (result == 1) ++wins;
        else if (result == 0) ++losses;
        else ++failures;
    }
    std::cout << wins << " victories, " << losses << " valid defeats, " << failures << " invalid or stalled matches\n";
    // The game must allow both winning and losing. It must not force every
    // strategy to win; require a real full victory and valid outcomes in all runs.
    return wins > 0 && failures == 0 ? 0 : 1;
}
