# pragma once

enum class EnemyType {
    EASY,
    MEDIUM,
    HARD
};

enum class EnemyState {
    MOVING, //enemy walking towards the player
    FIGHTING, //enemy is attacking the player
    DEAD //enemy is dead
// this file contains shared enums 
};

enum class ActionState {
    Idle,
    Moving,
    Jumping,
    Ducking,
    Attacking,
    Damaged,
    Defeated
};

enum class PlayerType {
    Melee,
    Ranged,
    Defense
};

enum class AttackType {
    Melee,
    Projectile,
    High,
    Low
};

struct Rect {
    float x;
    float y;
    float width;
    float height;
};