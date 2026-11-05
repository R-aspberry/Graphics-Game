#include "Economy.h"

Economy::Economy(){
    coins = 30;
}

int Economy::getCoins() {
    return coins;
}
bool Economy::spendCoins(int cost) {
   if (cost< coins ||cost==coins){
    coins-=cost;
    return true;
   }  
     return false;
    
}
void Economy::addCoins(int amount) {
    coins+=amount;
}

bool Economy::canAfford(int cost){
   if (cost< coins ||cost==coins){
    return true;
   }  
     return false;
}
