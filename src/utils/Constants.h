
#pragma once
#include "../world/Lane.h"

namespace GameConfig {

    inline constexpr int LANE_COUNT = 4;

    inline constexpr float MELEE_SPEED = 0.09f;
    inline constexpr float RANGED_SPEED = 0.08f;
    inline constexpr float DEFENSE_SPEED = 0.0f;

    inline constexpr float EASY_ENEMY_SPEED = 0.045f;
    inline constexpr float MEDIUM_ENEMY_SPEED = 0.045f;
    inline constexpr float HARD_ENEMY_SPEED = 0.08f;

    inline constexpr int MELEE_PLAYER_HP = 5;
    inline constexpr int RANGED_PLAYER_HP = 5;
    inline constexpr int DEFENSE_PLAYER_HP = 7;
    inline constexpr int MELEE_PLAYER_DAMAGE = 1;
    inline constexpr int RANGED_PLAYER_DAMAGE = 1;

    inline constexpr float MELEE_ATTACK_INTERVAL = 0.35f; // cooldown after the melee animation
    inline constexpr float RANGED_ATTACK_INTERVAL = 1.5f;

    inline constexpr float JUMP_DURATION = 1.0f;
    inline constexpr float DUCK_DURATION = 1.0f;
    inline constexpr float ATTACK_ANIMATION_DURATION = 0.5f;
    inline constexpr float PLAYER_PROJECTILE_SPEED = 0.5f;

    inline constexpr float EASY_ENEMY_ATTACK_INTERVAL = 4.0f;
    inline constexpr float MEDIUM_ENEMY_ATTACK_INTERVAL = 3.75f;
    inline constexpr float HARD_ENEMY_ATTACK_INTERVAL = 1.0f;

    inline constexpr float ENEMY_SPAWN_INTERVAL = 5.5f; // number of enemies per wave is unchanged

    inline constexpr int INITIAL_COINS = 30;
    inline constexpr int EASY_ENEMY_REWARD = 10; 
    inline constexpr int MEDIUM_ENEMY_REWARD = 15;
    inline constexpr int HARD_ENEMY_REWARD = 20;

    // enemy health
    inline constexpr int EASY_ENEMY_HP = 5;
    inline constexpr int MEDIUM_ENEMY_HP = 5;
    inline constexpr int HARD_ENEMY_HP = 7;

    inline constexpr int EASY_ENEMY_DAMAGE = 1;
    inline constexpr int MEDIUM_ENEMY_DAMAGE = 1;
    inline constexpr int HARD_ENEMY_DAMAGE = 2;

    inline constexpr int WAVE_1_COUNT = 3;
    inline constexpr int WAVE_2_COUNT = 5;
    inline constexpr int WAVE_3_COUNT = 10;

}

namespace WorldLayout {

    inline constexpr float PLAYER_WIDTH = 0.050f;
    inline constexpr float PLAYER_HEIGHT = 0.070f;
    inline constexpr float ENEMY_WIDTH = 0.055f;
    inline constexpr float ENEMY_HEIGHT = 0.070f;

    inline constexpr float PROJECTILE_WIDTH = 0.012f;
    inline constexpr float PROJECTILE_HEIGHT = 0.012f;
    inline constexpr float COLLECTIBLE_WIDTH = 0.025f;
    inline constexpr float COLLECTIBLE_HEIGHT = 0.025f;

    inline constexpr float PLAYER_SPAWN_INSET = 0.025f;
    inline constexpr float ENEMY_SPAWN_INSET = 0.020f;
    inline constexpr float HIGH_ATTACK_OFFSET = 0.25f;
    inline constexpr float LOW_ATTACK_OFFSET = 0.75f;
    inline constexpr float FLOOR_GAP = 0.008f;
    inline constexpr float HOME_BASE_X = 0.18f;

    inline float playerSpawnX(const Lane& lane) {
        return lane.left + PLAYER_SPAWN_INSET;
    }

    inline float enemySpawnX(const Lane& lane) {
        return lane.right - ENEMY_WIDTH - ENEMY_SPAWN_INSET;
    }

    inline float standingY(const Lane& lane, float characterHeight) {
        return lane.bottom - characterHeight - FLOOR_GAP;
    }

    // Convert top-left gameplay Y to bottom-left rendering Y.
    inline float toRenderY(float worldY, float objectHeight) {
        return 1.0f - worldY - objectHeight;
    }
}


// please refer to these notes 
// projectiles are not animated upon hits becuase i have asked menrit and she said translation is enough 