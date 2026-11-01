
#pragma once

#include "Enemy.h"

class EasyEnemy : public Enemy {
public:
    EasyEnemy(int lane, glm::vec2 position, float attackInterval);

    AttackType getAttackType() const override {
        return AttackType::High;
    }
    EnemyType getEnemyType() const override {
        return EnemyType::EASY;
    }
};
