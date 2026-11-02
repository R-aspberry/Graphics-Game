#include "Player.h"
#include "Projectile.h"
#include "RangedPlayer.h"

    Projectile::Projectile(float x , float y ,  float width , float height, int laneIndex, float speed,Player* player): Entity(x,y,width,height,laneIndex) , speed(speed) , player(player), hasHit(false), damage(1) {
        // A projectile can outlive its owner; combat uses this damage snapshot.
        if (const auto* ranged = dynamic_cast<const RangedPlayer*>(player)) {
            damage = ranged->getAttackDamage();
        }
    }

    Projectile::~Projectile() = default;

    void Projectile::update(float deltaTime) {
        if (!isActive() || deltaTime < 0.0f) return;
        x += speed*deltaTime;
    }

    Player* Projectile::getOwner() const{
        return this->player;
    }
    bool Projectile::hasAlreadyHit() const{
        return hasHit;
    }
    void Projectile::markHit() {
        hasHit = true;
        setActive(false);
    }
    float Projectile::getSpeed() const{
        return speed;
    }

