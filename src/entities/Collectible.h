#pragma once 
#include <glm/glm.hpp>

class Collectible {
    private:
        int lane; 
        glm::vec2 position;
        float widthofcollisionbox;
        float heightofcollisionbox;
        int valueofcoins;
        bool collected; 
    
    public:
        Collectible(int lane , glm::vec2 position, int reward);
        glm::vec2 getPosition() const { return position;};
        float getWidthOfCollisionBox() const { return widthofcollisionbox;};
        float getHeightOfCollisionBox() const { return heightofcollisionbox;};
        int getValueOfCoins() const { return valueofcoins;};
        bool isCollected() const { return collected;};
        int getLane() const { return lane;};
};
