# pragma once
# include "Enemy.h"

class HardEnemy : public Enemy {

    public:
        HardEnemy(int lane, glm::vec2 position, float attackInterval);

};