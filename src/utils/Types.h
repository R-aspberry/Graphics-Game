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
};