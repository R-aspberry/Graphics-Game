#pragma once 
#include "../world/Lane.h"

namespace WorldLayout {

    // Character hitbox sizes
    inline constexpr float PLAYER_WIDTH = 0.050f;
    inline constexpr float PLAYER_HEIGHT = 0.070f;

    inline constexpr float ENEMY_WIDTH = 0.055f;
    inline constexpr float ENEMY_HEIGHT = 0.070f;
    inline constexpr float LOW_ATTACK_OFFSET = 0.75f;
    inline constexpr float HIGH_ATTACK_OFFSET = 0.25f;

    // Projectile sizes
    inline constexpr float PROJECTILE_WIDTH = 0.012f;
    inline constexpr float PROJECTILE_HEIGHT = 0.012f;

    // Collectible size
    inline constexpr float COLLECTIBLE_WIDTH = 0.025f;
    inline constexpr float COLLECTIBLE_HEIGHT = 0.025f;

    // Horizontal spawn margins
    inline constexpr float PLAYER_SPAWN_INSET = 0.025f;
    inline constexpr float ENEMY_SPAWN_INSET = 0.020f;

    // Gap between character feet and lane bottom
    inline constexpr float FLOOR_GAP = 0.008f;

    // Enemy crossing this X reaches the home base
    inline constexpr float HOME_BASE_X = 0.18f;

    inline float playerSpawnX(const Lane& lane) {
        return lane.left + PLAYER_SPAWN_INSET;
    }

    inline float enemySpawnX(const Lane& lane) {
        return lane.right - ENEMY_WIDTH
                          - ENEMY_SPAWN_INSET;
    }

    // Top-left Y of a character standing in a lane
    inline float standingY(const Lane& lane, float characterHeight) {
        return lane.bottom - characterHeight - FLOOR_GAP;
    }

    // Convert top-left gameplay Y to bottom-left
    // dah 3lshan el rendering coordinate system
    inline float toRenderY(float worldY, float objectHeight) {
        return 1.0f - worldY - objectHeight;
    }
}
