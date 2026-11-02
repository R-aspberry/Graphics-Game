#include "Player.h"
#include "../utils/Constants.h"
#include <cmath>
#include <algorithm>

Player::Player(float x, float y, float width, float height, int laneIndex,
               int maxHealth, float moveSpeed)
    : Entity(x, y, width, height, laneIndex), health(maxHealth), maxHealth(maxHealth),
      moveSpeed(moveSpeed), actionState(ActionState::Idle), PrevactionState(ActionState::Moving),
      movementBlocked(false), jumpTimer(0), duckTimer(0), damageFlashTimer(0), attackRange(0) {}

Player::~Player() = default;

void Player::takeDamage(int damage) {
    if (!isAlive() || damage <= 0) return;
    health = damage >= health ? 0 : health - damage;
    damageFlashTimer = Constants::damageFlashDuration;
    // Damage feedback has its own timer; it must not cancel attacks or evasion.
    if (health == 0) {
        setActive(false);
        actionState = ActionState::Defeated;
    }
}
bool Player::isAlive() const { return health > 0 && actionState != ActionState::Defeated; }
int Player::getHealth() const { return health; }
int Player::getMaxHealth() const { return maxHealth; }
float Player::getMoveSpeed() const { return moveSpeed; }
ActionState Player::getActionState() const { return actionState; }
void Player::setMovementBlocked(bool blocked) { movementBlocked = blocked; }
bool Player::isMovementBlocked() const { return movementBlocked; }

void Player::jump() {
    if (!isAlive() || isJumping() || isDucking()) return;
    setActionState(ActionState::Jumping);
}
void Player::duck() {
    if (!isAlive() || isJumping() || isDucking()) return;
    setActionState(ActionState::Ducking);
}
bool Player::isJumping() const { return jumping; }
bool Player::isDucking() const { return ducking; }
void Player::updateMovement(float deltaTime) {
    if (std::isfinite(deltaTime) && deltaTime >= 0 && canMove()) x += moveSpeed * deltaTime;
}
void Player::updateJump(float deltaTime) {
    if (!jumping || !std::isfinite(deltaTime) || deltaTime < 0) return;
    jumpTimer += deltaTime;
    if (jumpTimer >= Constants::JUMP_DURATION) {
        jumping = false;
        jumpTimer = 0;
        if (isAlive()) actionState = ActionState::Moving;
    }
}
void Player::updateDuck(float deltaTime) {
    if (!ducking || !std::isfinite(deltaTime) || deltaTime < 0) return;
    duckTimer += deltaTime;
    if (duckTimer >= Constants::DUCK_DURATION) {
        ducking = false;
        duckTimer = 0;
        if (isAlive()) actionState = ActionState::Moving;
    }
}
void Player::updateDamageFeedback(float deltaTime) {
    if (!std::isfinite(deltaTime) || deltaTime < 0) return;
    damageFlashTimer = std::max(0.0f, damageFlashTimer - deltaTime);
}
void Player::update(float deltaTime) {
    if (!std::isfinite(deltaTime) || deltaTime < 0 || !isAlive()) return;
    updateMovement(deltaTime);
    updateJump(deltaTime);
    updateDuck(deltaTime);
    updateDamageFeedback(deltaTime);
    animation.update(deltaTime);
}
void Player::setActionState(ActionState newState) {
    if (!isAlive()) return;
    actionState = newState;
    jumping = newState == ActionState::Jumping;
    ducking = newState == ActionState::Ducking;
    if (jumping) jumpTimer = 0;
    if (ducking) duckTimer = 0;
}
bool Player::canMove() const {
    return active && isAlive() && !movementBlocked && actionState != ActionState::Idle;
}
