#define Entity_H

#include "Types.h"

class Entity{
    public :
        float x;
        float y;
        float width;
        float height;
        int laneIndex;
        bool active;
        Entity (float x, float y, float width, float height, int laneIndex) ;

    virtual ~Entity() ;

    float getX();
    float getY();
    float getWidth() ;
    float getHeight() ;
    int getLaneIndex();
    bool isActive() ;

    void setPosition(float newX, float newY) ;
    void setActive(bool isActive) ;
    Rect getBounds() ;

    virtual void update(float deltaTime) = 0;
     



};