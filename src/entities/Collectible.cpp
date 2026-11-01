#include "Collectible.h"

Collectible::Collectible(int lane , glm::vec2 position, int reward)
                : Entity(position.x, position.y, 20.0f, 20.0f,lane),
                valueofcoins(reward), 
                collected(false) {}

bool Collectible::collect(){
    if (collected) {
        return false;
    }
    collected = true;
    return true;
}

void Collectible::update(float deltaTime) {
    (void)deltaTime;
}
