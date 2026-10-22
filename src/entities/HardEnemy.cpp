# include "HardEnemy.h"
# include "../utils/Constants.h"

HardEnemy::HardEnemy(int lane, glm::vec2 position, float attackInterval)
    : Enemy(GameConfig::HARD_ENEMY_HP, GameConfig::HARD_ENEMY_DAMAGE, GameConfig::HARD_ENEMY_SPEED, attackInterval, lane, position) {}
    