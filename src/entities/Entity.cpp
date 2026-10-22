// this class has is the parent class for both the players and the enemies 
#include <Types.h>
#include <Entity.h>

class Entity {
float x ; // x position of the entity
float y ; // y position of the entity
float width ; // width of the entity
float height ; // height of the entity
int laneIndex ; // index of the lane the entity is in
bool active;

Entity (float x, float y, float width, float height, int laneIndex) {
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->laneIndex = laneIndex;
    this->active = true; // initially player / enemy is active 
}

    virtual ~Entity() {
        // destroyes the object
    }

    float getX()  { return x; }
    float getY()  { return y; }
    float getWidth()  { return width; }
    float getHeight()  { return height; }
    int getLaneIndex()  { return laneIndex; }
    bool isActive()  { return active; }

    void setPosition(float newX, float newY) {
        x = newX;
        y = newY;
    }
    void setActive(bool isActive) {
        active = isActive;
    }
    Rect getBounds()  {
        Rect R = {x,y,width,height};
        return R;
    }

    virtual void update(float deltaTime) = 0;
};
