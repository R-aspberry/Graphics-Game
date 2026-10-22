#pragma once
#include <glm/glm.hpp>
#include "../utils/Types.h"

class Enemy{
    private : 
        int health;
        int damage;
        float speed;
        float attackInterval;
        float timeSinceLastAttack;
        int lane;
        int maxHealth;
        glm::vec2 position;
        EnemyState state;
        void moveTowardsBase(float deltaTime);
        void updateAttackTimer(float deltaTime);
    
    public:
        Enemy(int health, int damage, float speed, float attackInterval, int lane, glm::vec2 position);
        virtual ~Enemy() = default;
        int getHealth() const { return health; }
        int getDamage() const { return damage; }
        float getSpeed() const { return speed; }
        float getAttackInterval() const { return attackInterval; }
        float getTimeSinceLastAttack() const { return timeSinceLastAttack; }
        int getLane() const { return lane; }
        int getMaxHealth() const { return maxHealth; }
        glm::vec2 getPosition() const { return position; }
        EnemyState getState() const { return state; }
        void update(float deltaTime);
        void takeDamage(int incomingDamage);
        bool canAttack() const;
        void resetAttackTimer();
        void setState(EnemyState newState); 
};