# pragma once
# include "Enemy.h"

class MediumEnemy : public Enemy {

    public:
        MediumEnemy(int lane, glm::vec2 position, float attackInterval);

};