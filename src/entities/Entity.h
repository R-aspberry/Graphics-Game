#ifndef ENTITY_H
#define ENTITY_H

#include "../utils/Types.h"

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

    float getX() const;
    float getY() const;
    float getWidth() const;
    float getHeight() const;
    int getLaneIndex()const;
    bool isActive()const;

    void setPosition(float newX, float newY) ;
    void setActive(bool isActive) ;
    Rect getBounds() const;

    virtual void update(float deltaTime) = 0;
     



};

#endif