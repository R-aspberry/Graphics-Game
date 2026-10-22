// this file contains shared enums 

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