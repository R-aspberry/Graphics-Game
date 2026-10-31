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

float Entity::getX() {
    return x;
}

float Entity::getY() {
    return y;
}

float Entity::getWidth() {
    return width;
}

float Entity::getHeight() {
    return height;
}

int Entity::getLaneIndex() {
    return laneIndex;
}

bool Entity::isActive() {
    return active;
}


void Entity::setPosition(float newX, float newY) {
    x = newX;
    y = newY;
}

void Entity::setActive(bool isActive) {
    active = isActive;
}

Rect Entity::getBounds() {
    return {x, y, width, height};
}
