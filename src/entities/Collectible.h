#pragma once 
#include <glm/glm.hpp>
#include "../entities/Entity.h"

class Collectible: public Entity {
    private: 
        int valueofcoins;
        bool collected;
    
    public:
        Collectible(int lane , glm::vec2 position, int reward);

        glm::vec2 getPosition() const {
        return glm::vec2(x, y);
        }

        float getWidthOfCollisionBox() const {
            return width;
        }

        float getHeightOfCollisionBox() const {
            return height;
        }

        int getValueOfCoins() const { return valueofcoins;};
        bool isCollected() const { return collected;};
        int getLane() const { return laneIndex;};
        bool collect();
        void update(float deltaTime) override;
};
