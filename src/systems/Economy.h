
#pragma once

class Economy {
private:
    int coins;

public:
    Economy();
    // Return current coin balance
    int getCoins();

    bool canAfford(int cost);

    // Buy an item if the player has enough coins
    bool spendCoins(int cost);

    // Reward player with coins
    void addCoins(int amount);
};
