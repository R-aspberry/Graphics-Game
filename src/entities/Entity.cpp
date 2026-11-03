#include "Entity.h"

Entity::Entity(float x, float y, float width, float height, int laneIndex)
    : x(x),
      y(y),
      width(width),
      height(height),
      laneIndex(laneIndex),
      active(true) {
}

Entity::~Entity() = default;

float Entity::getX() const{
    return x;
}

float Entity::getY() const{
    return y;
}

float Entity::getWidth() const{
    return width;
}

float Entity::getHeight() const{
    return height;
}

int Entity::getLaneIndex() const{
    return laneIndex;
}

bool Entity::isActive() const{
    return active;
}


void Entity::setPosition(float newX, float newY) {
    x = newX;
    y = newY;
}

void Entity::setActive(bool isActive) {
    active = isActive;
}

Rect Entity::getBounds() const{
    return {x, y, width, height};
}
