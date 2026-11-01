#include "Collectible.h"

Collectible::Collectible(int lane , glm::vec2 position, int reward)
    : lane(lane), position(position), widthofcollisionbox(20.0f), heightofcollisionbox(20.0f), valueofcoins(reward), collected(false) {}

bool Collectible::collect(){
    if (collected) {
        return false;
    }
    collected = true;
    return true;
}

