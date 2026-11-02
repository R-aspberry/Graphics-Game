#include "Collectible.h"
#include "../utils/WorldLayout.h"

Collectible::Collectible(int lane , glm::vec2 position, int reward)
                : Entity(position.x, position.y, WorldLayout::COLLECTIBLE_WIDTH, WorldLayout::COLLECTIBLE_HEIGHT, lane),
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
