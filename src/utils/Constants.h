
#pragma once

namespace GameConfig {

    // Playfield
    inline constexpr int LANE_COUNT = 4;

    // Player movement speeds
    inline constexpr float MELEE_SPEED = 100.0f;
    inline constexpr float RANGED_SPEED = 90.0f;

    // Enemy movement speeds
    inline constexpr float EASY_ENEMY_SPEED = 60.0f;
    inline constexpr float MEDIUM_ENEMY_SPEED = 60.0f;
    inline constexpr float HARD_ENEMY_SPEED = 110.0f;

    // Attack intervals
    inline constexpr float MELEE_ATTACK_INTERVAL = 1.0f;
    inline constexpr float RANGED_ATTACK_INTERVAL = 1.5f;

    // Enemy spawning
    inline constexpr float ENEMY_SPAWN_INTERVAL = 3.0f;

    // Economy
    inline constexpr int INITIAL_COINS = 50;

    // Required enemy health
    inline constexpr int EASY_ENEMY_HP = 5;
    inline constexpr int MEDIUM_ENEMY_HP = 5;
    inline constexpr int HARD_ENEMY_HP = 7;

    // Enemy damage
    inline constexpr int EASY_ENEMY_DAMAGE = 1;
    inline constexpr int MEDIUM_ENEMY_DAMAGE = 1;
    inline constexpr int HARD_ENEMY_DAMAGE = 2;

    // Waves
    inline constexpr int WAVE_1_COUNT = 3;
    inline constexpr int WAVE_2_COUNT = 5;
    inline constexpr int WAVE_3_COUNT = 10;
}
namespace Constants
{
    constexpr float JUMP_DURATION = 0.6f;
    constexpr float DUCK_DURATION = 0.8f;
    constexpr float damageFlashDuration = 0.6f;
    constexpr float attackDuration = 0.5f;
    constexpr float attackIntervals = 0.5f ; 
    constexpr float ProjectileSpeed = 0.5f;
    constexpr float homebasex = 0.0f;
    constexpr float homebasey= 0.0f;
    constexpr float defensewidth = 2.0f;
    constexpr float defenseheight = 3.0f;
    constexpr float meleewidth = 2.0f;
    constexpr float meleeheight = 3.0f;
    constexpr float rangedwidth = 2.0f;
    constexpr float rangedheight = 3.0f;
    constexpr float MaxHealthMelee = 5;
    constexpr float MaxHealthDefense = 7;
    constexpr float MaxHealthRanged = 5;
    constexpr float speedMelee = 5;
    constexpr float speedDefense = 5;
    constexpr float speedRanged = 5;
    constexpr float ProjectileWidth = 5;
    constexpr float ProjectileHeight = 5;

}
