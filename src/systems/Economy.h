#pragma once
#include "../utils/Types.h"

class Economy {
private:
    int coins;
    int collectedCoins;
public:
    Economy();
    int getCoins() const;
    bool canAfford(int cost) const;
    bool spendCoins(int cost);
    void addCoins(int amount);
    int getPlayerCost(PlayerType type) const;
    bool isUnlocked(PlayerType type) const;
    void reset();
};
