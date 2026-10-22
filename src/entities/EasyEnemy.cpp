# include "EasyEnemy.h"
# include "../utils/Constants.h"

EasyEnemy::EasyEnemy(int lane, glm::vec2 position, float attackInterval)
    : Enemy(GameConfig::EASY_ENEMY_HP, GameConfig::EASY_ENEMY_DAMAGE, GameConfig::EASY_ENEMY_SPEED, attackInterval, lane, position) {}
    