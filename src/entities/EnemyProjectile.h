#pragma once

#include "Entity.h"
#include "../utils/Types.h"

class EnemyProjectile : public Entity {
    private:
        float speed;
        AttackType attackType;
        int damage;

    public: 
        EnemyProjectile(float x,float y,int lane, float speed,int damage, AttackType attackType);
        static constexpr float WIDTH = 0.012f;
        static constexpr float HEIGHT = 0.012f;
        float getSpeed() const { return speed; }
        AttackType getAttackType() const { return attackType; }
        int getDamage() const { return damage; }

        void update(float deltaTime) override;
};