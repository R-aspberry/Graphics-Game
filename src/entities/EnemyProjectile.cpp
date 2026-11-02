#include "../entities/EnemyProjectile.h"

EnemyProjectile::EnemyProjectile(float x,float y,int lane, float speed,int damage, AttackType attackType)
    : Entity(x, y, WIDTH, HEIGHT, lane), 
      speed(speed), 
      damage(damage), 
      attackType(attackType) {}

void EnemyProjectile::update(float deltaTime) {
    if (!isActive() || deltaTime < 0.0f) {
        return;
    }
    x = x - speed * deltaTime;
}