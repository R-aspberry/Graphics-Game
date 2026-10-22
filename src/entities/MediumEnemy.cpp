# include "MediumEnemy.h"
# include "../utils/Constants.h"

MediumEnemy::MediumEnemy(int lane, glm::vec2 position, float attackInterval)
    : Enemy(GameConfig::MEDIUM_ENEMY_HP, GameConfig::MEDIUM_ENEMY_DAMAGE, GameConfig::MEDIUM_ENEMY_SPEED, attackInterval, lane, position) {}
    