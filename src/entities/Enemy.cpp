#include "Enemy.h"

Enemy::Enemy(int health, int damage, float speed, float attackInterval, int lane, glm::vec2 position)
    : health(health), damage(damage), speed(speed), attackInterval(attackInterval), timeSinceLastAttack(0.0f), lane(lane), maxHealth(health), position(position), state(EnemyState::MOVING) {}

void Enemy:: update(float deltaTime) {
    if (deltaTime < 0.0f) {
        return;
    }
    if (state == EnemyState::MOVING)
        moveTowardsBase(deltaTime);
    if (state != EnemyState::DEAD)
        updateAttackTimer(deltaTime);
}

void Enemy::moveTowardsBase(float deltaTime) {
    position.x = position.x - speed * deltaTime;
}

void Enemy::takeDamage(int incomingDamage){
    if (state == EnemyState::DEAD)
        return;
    if (incomingDamage > 0){
        health -= incomingDamage;
        if (health <= 0){
            health = 0;
            state = EnemyState::DEAD;
        }
    }
}

void Enemy::updateAttackTimer(float deltaTime) {
    if (state == EnemyState::DEAD)  return;
    timeSinceLastAttack += deltaTime;  
}

bool Enemy::canAttack() const {
    if(state == EnemyState::DEAD) return false;
    if(timeSinceLastAttack >= attackInterval)
    {
        return true;
    }
    return false;
}

void Enemy::resetAttackTimer() {
    timeSinceLastAttack = 0.0f;
}

void Enemy::setState(EnemyState newState) {
    if (state == EnemyState::DEAD|| newState == EnemyState::DEAD) return; 
    state = newState;
}