#include "Game.h"
#include "entities/MeleePlayer.h"
#include "entities/RangedPlayer.h"
#include "entities/DefensePlayer.h"
#include "utils/Constants.h"
#include <cmath>
#include <iostream>
#include <limits>
#include <memory>

int passed = 0, failed = 0;
void check(bool condition, const char* description) {
    (condition ? ++passed : ++failed);
    std::cout << (condition ? "[PASS] " : "[FAIL] ") << description << '\n';
}
bool near(float a, float b) { return std::fabs(a - b) < .0001f; }

void placementAndEconomy() {
    GameManager manager;
    check(!manager.placePlayer(PlayerType::Melee, 0), "Missing grid rejects placement safely");
    Grid grid;
    manager.setGrid(&grid);
    check(!manager.placePlayer(PlayerType::Melee, -1) && !manager.placePlayer(PlayerType::Melee, 4),
          "Invalid lanes reject placement safely");
    check(!manager.placePlayer(static_cast<PlayerType>(99), 0), "Invalid player type rejects placement");
    check(manager.getPlayers().empty(), "Invalid placements create no players");
    Economy economy;
    check(!economy.spendCoins(-10) && !economy.canAfford(-10), "Negative costs are rejected");
    economy.addCoins(-10);
    check(economy.getCoins() == 30, "Negative reward cannot reduce balance");
    check(economy.isUnlocked(PlayerType::Melee) && !economy.isUnlocked(PlayerType::Ranged) &&
          !economy.isUnlocked(PlayerType::Defense), "Only melee is unlocked initially");
    economy.addCoins(20);
    check(economy.isUnlocked(PlayerType::Ranged) && !economy.isUnlocked(PlayerType::Defense),
          "Twenty earned coins unlock ranged");
    economy.spendCoins(40);
    check(!economy.isUnlocked(PlayerType::Ranged), "Ranged card relocks when balance falls below its cost");
    economy.addCoins(10);
    check(economy.isUnlocked(PlayerType::Ranged) && !economy.isUnlocked(PlayerType::Defense),
          "Cards require both earned unlock progress and enough current coins");
    economy.addCoins(10);
    check(economy.isUnlocked(PlayerType::Defense), "Defense unlocks when progress and balance meet its cost");
    economy.addCoins(std::numeric_limits<int>::max());
    check(economy.getCoins() == std::numeric_limits<int>::max(), "Large rewards cannot overflow balance");
    Game game;
    check(!game.purchasePlayer(PlayerType::Melee, 9) && game.getEconomy().getCoins() == 30,
          "Invalid purchase never spends coins");
    check(!game.purchasePlayer(PlayerType::Ranged, 0), "Locked purchase is rejected");
    check(game.purchasePlayer(PlayerType::Melee, 0) && game.purchasePlayer(PlayerType::Melee, 1) &&
          game.purchasePlayer(PlayerType::Melee, 2) && game.getEconomy().getCoins() == 0,
          "Starting balance buys exactly three melee players");
    check(!game.purchasePlayer(PlayerType::Melee, 3), "Insufficient balance rejects purchase");
    game.getEconomy().addCoins(30);
    Player* defense = game.purchasePlayer(PlayerType::Defense, 3);
    check(defense && defense->getPlayerType() == PlayerType::Defense,
          "Defense purchase creates an actual defense player");
}
void actionsAndMovement() {
    Game game;
    auto* ranged = dynamic_cast<RangedPlayer*>(game.placePlayer(PlayerType::Ranged, 0));
    ranged->startAttack(game.getGameManager());
    float before = ranged->x;
    ranged->update(.1f);
    check(ranged->x > before && ranged->isAttacking(), "Ranged player moves during an attack");
    ranged->jump();
    check(ranged->isJumping() && ranged->isAttacking(), "Jump can start during attack animation");
    ranged->takeDamage(1);
    check(ranged->isJumping(), "Damage feedback does not cancel evasion");
    ranged->update(.5f);
    check(!ranged->isAttacking() && ranged->isJumping(), "Attack ends without cancelling jump");
    ranged->startAttack(game.getGameManager());
    check(game.getGameManager().projectiles.size() == 1, "Evasion cannot bypass attack cooldown");
    ranged->update(.9f);
    check(ranged->canAttack(), "Ranged cooldown measures time between attack starts");
    ranged->duck();
    ranged->startAttack(game.getGameManager());
    check(ranged->isDucking() && ranged->isAttacking(), "Attacks can continue while ducking");
    game.getGameManager().update(100);
    check(near(ranged->x + ranged->width, game.getGrid().getLanes()[0].right),
          "Movement stops at the lane right edge");
    float edge = ranged->x;
    game.getGameManager().update(-1);
    check(ranged->x == edge, "Invalid update cannot move players backwards");

    Game selected;
    Player* first = selected.placePlayer(PlayerType::Melee, 0);
    Player* second = selected.placePlayer(PlayerType::Ranged, 1);
    Player* third = selected.placePlayer(PlayerType::Melee, 2);
    Player* shield = selected.placePlayer(PlayerType::Defense, 0);
    selected.getGrid().toggleLaneSelection(0);
    selected.getGrid().toggleLaneSelection(1);
    selected.jumpSelectedLanes();
    check(first->isJumping() && second->isJumping() && !third->isJumping() && !shield->isJumping(),
          "Selected lane commands affect all mobile players and ignore defenders");
    first->setActionState(ActionState::Moving);
    second->setActionState(ActionState::Moving);
    selected.duckSelectedLanes();
    check(first->isDucking() && second->isDucking() && !third->isDucking(),
          "Duck command respects multiple selected lanes");
}
void dropsResetAndTiming() {
    Game game;
    game.getWaveManager().update(3);
    Enemy* enemy = game.getWaveManager().getEnemyAt(0);
    int lane = enemy->getLane();
    enemy->takeDamage(100);
    game.getWaveManager().finishUpdate();
    check(game.getWaveManager().getDropCount() == 1, "Dead enemy produces a collectible");
    Collectible* drop = game.getWaveManager().getDropAt(0);
    Player* player = game.placePlayer(PlayerType::Melee, lane);
    player->setPosition(drop->x, drop->y);
    game.collectDrops();
    check(game.getEconomy().getCoins() == GameConfig::INITIAL_COINS + GameConfig::EASY_ENEMY_REWARD && game.getWaveManager().getDropCount() == 0,
          "Player collision awards reward and removes collectible");
    game.collectDrops();
    check(game.getEconomy().getCoins() == GameConfig::INITIAL_COINS + GameConfig::EASY_ENEMY_REWARD, "Collected reward cannot be awarded twice");
    game.getGrid().toggleLaneSelection(lane);
    game.setPaused(true);
    float x = player->x;
    game.update(10);
    check(player->x == x, "Paused game does not advance simulation");
    game.reset();
    check(game.getGameManager().getPlayers().empty() && game.getGameManager().projectiles.empty() &&
          game.getWaveManager().getActiveEnemyCount() == 0 && game.getWaveManager().getDropCount() == 0 &&
          game.getWaveManager().getEnemyProjectilesCount() == 0 && game.getWaveManager().getCurrentWave() == 1 &&
          game.getEconomy().getCoins() == 30 && !game.isPaused() && !game.hasLost() && !game.hasWon() &&
          !game.getGrid().getLanes()[lane].selected, "Reset clears real simulation state and selections");
    game.startCutscene();
    game.update(6);
    check(game.isCutscenePlaying() && game.getWaveManager().getActiveEnemyCount() == 0,
          "Intro runs without advancing gameplay");
    game.update(6);
    check(!game.isCutscenePlaying(), "Intro ends automatically after twelve seconds");
    game.update(-1);
    game.update(std::numeric_limits<float>::quiet_NaN());
    check(game.getWaveManager().getActiveEnemyCount() == 0, "Invalid frame times do not change game state");

    Game large, small;
    Player* a = large.placePlayer(PlayerType::Ranged, 0);
    Player* b = small.placePlayer(PlayerType::Ranged, 0);
    large.update(2);
    for (int i = 0; i < 240; ++i) small.update(1.0f / 120);
    check(near(a->x, b->x) && large.getGameManager().projectiles.size() == small.getGameManager().projectiles.size(),
          "Large frame uses the same fixed simulation steps as small frames");
}
void terminalStates() {
    Game loss;
    loss.update(30);
    check(loss.hasLost(), "An enemy reaching the home base ends the game");
    float x = loss.getWaveManager().getEnemyAt(0)->getPosition().x;
    loss.update(10);
    check(loss.getWaveManager().getEnemyAt(0)->getPosition().x == x,
          "Simulation remains frozen after defeat");
    check(!loss.purchasePlayer(PlayerType::Melee, 0), "Cannot purchase after defeat");
    loss.reset();
    check(!loss.hasLost(), "Reset clears defeat");
    Game win;
    for (int wave = 1; wave <= 3; ++wave) {
        auto& waves = win.getWaveManager();
        waves.update(wave == 1 ? 9 : wave == 2 ? 15 : 30);
        for (std::size_t i = 0; i < waves.getActiveEnemyCount(); ++i) waves.getEnemyAt(i)->takeDamage(100);
        win.update(1.0f / 120);
    }
    check(win.hasWon() && !win.hasLost(), "Defeating all three wave quotas triggers victory");
    std::size_t count = win.getWaveManager().getDropCount();
    win.update(30);
    check(win.getWaveManager().getDropCount() == count, "Victory freezes spawning and death processing");
    win.reset();
    check(!win.hasWon() && win.getWaveManager().getCurrentWave() == 1, "Reset clears victory");
}
void projectileCleanupOrder() {
    GameManager players;
    Grid grid;
    players.setGrid(&grid);
    WaveManager waves;
    waves.update(3);
    Enemy* enemy = waves.getEnemyAt(0);
    Player* target = players.placePlayer(PlayerType::Melee, enemy->getLane());
    target->x = .4f;
    waves.update(0); // Create the first real enemy projectile at the spawn edge.
    waves.update(10, true);
    check(waves.getEnemyProjectilesCount() > 0 && waves.getEnemyProjectileAt(0)->x < 0,
          "Deferred cleanup retains shots which crossed offscreen during movement");
    CollisionSystem collision;
    CombatSystem combat(&players, &collision);
    combat.update(10, waves);
    check(target->getHealth() == 4, "Offscreen shot is collision-tested before expiration");
    waves.finishUpdate();
    check(waves.getEnemyProjectilesCount() == 0, "Expired enemy shots are removed after combat");
}
void edgeCombatAndCardLocks() {
    Game game;
    game.getWaveManager().setRandomSeed(42);
    game.getWaveManager().update(3);
    Enemy* enemy = game.getWaveManager().getEnemyAt(0);
    Player* ranged = game.placePlayer(PlayerType::Ranged, enemy->getLane());
    game.getGameManager().update(100); // Advance the player to the actual lane boundary.
    game.update(1.7f);
    check(enemy->getHealth() <= 3, "Ranged shots hit overlapping enemies at the right boundary");
    check(ranged->getHealth() < 5, "Overlapping ranged enemies can still hit players at the right boundary");

    Economy economy;
    economy.spendCoins(30);
    check(!economy.isUnlocked(PlayerType::Melee), "Melee card also locks when it becomes unaffordable");
    economy.addCoins(30);
    check(economy.isUnlocked(PlayerType::Defense), "Defense unlocks with earned progress and sufficient balance");
    economy.spendCoins(12);
    check(!economy.isUnlocked(PlayerType::Defense) && !economy.isUnlocked(PlayerType::Ranged),
          "Advanced cards automatically relock after spending");
    economy.addCoins(12);
    check(economy.isUnlocked(PlayerType::Defense) && economy.isUnlocked(PlayerType::Ranged),
          "Earning coins automatically unlocks eligible cards again");
    check(GameConfig::WAVE_1_COUNT * GameConfig::EASY_ENEMY_REWARD >= economy.getPlayerCost(PlayerType::Melee),
          "First wave rewards can fund an otherwise undefended fourth lane");
}
int main() {
    placementAndEconomy();
    actionsAndMovement();
    dropsResetAndTiming();
    terminalStates();
    projectileCleanupOrder();
    edgeCombatAndCardLocks();
    std::cout << passed << " passed, " << failed << " failed\n";
    return failed ? 1 : 0;
}
