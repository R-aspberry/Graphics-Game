
#pragma once

class Economy {
private:
    int coins;

public:
    // Player starts with 30 coins
    Economy() : coins(30) {}

    // Return current coin balance
    int getCoins() const {
        return coins;
    }

    // Check if player can afford an item
    bool canAfford(int cost) const {
        return cost >= 0 && coins >= cost;
    }

    // Buy an item if the player has enough coins
    bool spendCoins(int cost) {
        if (!canAfford(cost)) {
            return false;
        }

        coins -= cost;
        return true;
    }

    // Reward player with coins
    void addCoins(int amount) {
        if (amount > 0) {
            coins += amount;
        }
    }
};
