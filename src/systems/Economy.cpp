#include "Economy.h"
#include "../utils/Constants.h"
#include <limits>

Economy::Economy() : coins(GameConfig::INITIAL_COINS), collectedCoins(0) {}
int Economy::getCoins() const { return coins; }
bool Economy::canAfford(int cost) const { return cost >= 0 && coins >= cost; }
bool Economy::spendCoins(int cost) {
    if (!canAfford(cost)) return false;
    coins -= cost;
    return true;
}
void Economy::addCoins(int amount) {
    if (amount <= 0) return;
    const int limit = std::numeric_limits<int>::max();
    coins = amount > limit - coins ? limit : coins + amount;
    collectedCoins = amount > limit - collectedCoins ? limit : collectedCoins + amount;
}
int Economy::getPlayerCost(PlayerType type) const {
    if (type == PlayerType::Melee) return 10;
    if (type == PlayerType::Ranged) return 20;
    if (type == PlayerType::Defense) return 30;
    return -1;
}
bool Economy::isUnlocked(PlayerType type) const {
    // The starting coins unlock melee only. Earn rewards to unlock the other
    // roles; every card locks again whenever its current cost is unaffordable.
    if (!canAfford(getPlayerCost(type))) return false;
    if (type == PlayerType::Melee) return true;
    if (type == PlayerType::Ranged) return collectedCoins >= 20;
    if (type == PlayerType::Defense) return collectedCoins >= 30;
    return false;
}
void Economy::reset() { coins = GameConfig::INITIAL_COINS; collectedCoins = 0; }
