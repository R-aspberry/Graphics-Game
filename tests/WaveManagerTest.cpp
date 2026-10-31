#include "../src/systems/WaveManager.h"
#include "../src/entities/EasyEnemy.h"
#include "../src/entities/MediumEnemy.h"
#include "../src/entities/HardEnemy.h"
#include "../src/utils/Constants.h"

#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <type_traits>
#include <utility>

namespace {

int passed = 0;
int failed = 0;

void check(bool condition, const std::string& description) {
    if (condition) {
        ++passed;
        std::cout << "[PASS] " << description << '\n';
    } else {
        ++failed;
        std::cerr << "[FAIL] " << description << '\n';
    }
}

bool near(float a, float b) {
    return std::fabs(a - b) < 0.001f;
}

bool samePosition(const glm::vec2& a, const glm::vec2& b) {
    return near(a.x, b.x) && near(a.y, b.y);
}

bool validLaneAndSpawn(const Enemy* enemy) {
    if (!enemy) return false;
    const int lane = enemy->getLane();
    return lane >= 0 && lane < GameConfig::LANE_COUNT &&
           near(enemy->getPosition().x, 900.0f) &&
           near(enemy->getPosition().y, 100.0f + lane * 75.0f);
}

bool isEasy(const Enemy* e) {
    return dynamic_cast<const EasyEnemy*>(e) != nullptr;
}
bool isMedium(const Enemy* e) {
    return dynamic_cast<const MediumEnemy*>(e) != nullptr;
}
bool isHard(const Enemy* e) {
    return dynamic_cast<const HardEnemy*>(e) != nullptr;
}

bool validTypeForWave(int wave, const Enemy* enemy) {
    if (!enemy) return false;
    if (wave == 1) return isEasy(enemy);
    if (wave == 2) return isEasy(enemy) || isMedium(enemy);
    if (wave == 3) return isEasy(enemy) || isMedium(enemy) || isHard(enemy);
    return false;
}

void defeatAll(WaveManager& manager) {
    // WaveManager owns the enemies; do not erase them while iterating.
    for (std::size_t i = 0; i < manager.getActiveEnemyCount(); ++i) {
        Enemy* enemy = manager.getEnemyAt(i);
        if (enemy) enemy->takeDamage(1000);
    }
}

void checkInitialState() {
    std::cout << "\n--- Initial state / getters ---\n";
    WaveManager manager;
    check(manager.getCurrentWave() == 1, "Starts at Wave 1");
    check(manager.getEnemiesSpawnedInWave() == 0, "Starts with zero spawned enemies");
    check(manager.getActiveEnemyCount() == 0, "Starts with an empty enemy vector");
    check(!manager.hasWon(), "Has not won initially");
    check(manager.getEnemyAt(0) == nullptr, "Index 0 is invalid when vector empty");
    check(manager.getEnemyAt(100) == nullptr, "Large index is invalid when vector empty");

    static_assert(
        std::is_same<decltype(std::declval<WaveManager&>().getEnemyAt(0)), Enemy*>::value,
        "Non-const WaveManager must provide mutable Enemy* getter");
    static_assert(
        std::is_same<decltype(std::declval<const WaveManager&>().getEnemyAt(0)), const Enemy*>::value,
        "Const WaveManager must provide const Enemy* getter");
    const WaveManager& readOnly = manager;
    check(readOnly.getEnemyAt(0) == nullptr, "Const getter handles invalid index");

    check(manager.getDropCount() == 0, "New game starts with zero collectibles");
    check(manager.getDropAt(0) == nullptr, "Empty drop list rejects index zero");
    check(manager.getDropAt(9999) == nullptr, "Empty drop list rejects large index");
    check(!manager.removeDropAt(0), "Cannot remove a drop from an empty list");
    check(readOnly.getDropAt(0) == nullptr, "Drop getter works on const WaveManager");
}

void checkTimeBoundaries() {
    std::cout << "\n--- Spawn timing / exact boundaries ---\n";
    WaveManager manager;
    manager.update(0.0f);
    check(manager.getEnemiesSpawnedInWave() == 0, "Zero time does not spawn");
    manager.update(2.5f);
    check(manager.getEnemiesSpawnedInWave() == 0, "2.5 seconds is below spawn interval");
    manager.update(-50.0f);
    check(manager.getEnemiesSpawnedInWave() == 0, "Negative time before spawn is ignored");
    manager.update(0.5f);
    check(manager.getEnemiesSpawnedInWave() == 1, "Exactly 3 seconds spawns first enemy");

    const Enemy* first = manager.getEnemyAt(0);
    check(isEasy(first), "Wave 1's first enemy is EasyEnemy");
    check(validLaneAndSpawn(first), "First enemy spawns in a valid lane and position");
    check(first && near(first->getTimeSinceLastAttack(), first->getAttackInterval()),
          "Newly spawned enemy's attack timer starts at its interval");
    check(first && first->canAttack(), "Newly spawned enemy can attack immediately");
    check(manager.getDropCount() == 0, "Spawning an enemy does not create a drop");

    manager.update(2.5f);
    check(manager.getEnemiesSpawnedInWave() == 1, "Another 2.5 seconds does not spawn early");
    manager.update(0.5f);
    check(manager.getEnemiesSpawnedInWave() == 2, "Second enemy spawns at 6 seconds");
    manager.update(3.0f);
    check(manager.getEnemiesSpawnedInWave() == 3, "Third enemy spawns at 9 seconds");
    manager.update(30.0f);
    check(manager.getEnemiesSpawnedInWave() == 3 && manager.getActiveEnemyCount() == 3,
          "Wave 1 never exceeds three enemies");
    check(manager.getCurrentWave() == 1, "Wave 1 does not advance while enemies are alive");
    check(manager.getEnemyAt(3) == nullptr, "Index exactly equal to size is invalid");
    check(manager.getEnemyAt(9999) == nullptr, "Very large index is invalid");
    check(manager.getDropCount() == 0, "Living enemies do not generate drops over time");

    WaveManager largeFrame;
    largeFrame.update(9.0f);
    check(largeFrame.getActiveEnemyCount() == 3,
          "One 9-second update spawns three enemies");
    bool allStartAtEdge = true;
    for (std::size_t i = 0; i < largeFrame.getActiveEnemyCount(); ++i) {
        allStartAtEdge &= validLaneAndSpawn(largeFrame.getEnemyAt(i));
    }
    check(allStartAtEdge, "All new enemies start at spawn edge in same large frame");
    largeFrame.update(0.0f);
    check(largeFrame.getActiveEnemyCount() == 3,
          "Zero-time update does not duplicate spawns");
}

void checkMovementAndStates() {
    std::cout << "\n--- Movement / negative delta / state ---\n";
    WaveManager manager;
    manager.update(3.0f);
    Enemy* enemy = manager.getEnemyAt(0);
    check(enemy != nullptr, "Can access existing enemy by mutable getter");
    if (!enemy) return;

    // An enemy starts ready for its first attack. Reset to test the next cooldown.
    enemy->resetAttackTimer();
    const float laneY = enemy->getPosition().y;
    manager.update(0.5f);
    check(near(enemy->getPosition().x, 870.0f),
          "EasyEnemy moves left 30 units in 0.5 s");
    check(near(enemy->getPosition().y, laneY), "Movement does not change lane Y");
    check(near(enemy->getTimeSinceLastAttack(), 0.5f),
          "WaveManager updates enemy attack timer");
    enemy->setState(EnemyState::FIGHTING);
    const float beforeFight = enemy->getPosition().x;
    manager.update(1.0f);
    check(near(enemy->getPosition().x, beforeFight), "FIGHTING enemy does not move");
    check(near(enemy->getTimeSinceLastAttack(), 1.5f),
          "FIGHTING enemy continues attack timer");
    enemy->setState(EnemyState::MOVING);
    manager.update(0.5f);
    check(near(enemy->getPosition().x, 840.0f), "MOVING state resumes movement");

    const float beforeNegative = enemy->getPosition().x;
    const float attackTimeBeforeNegative = enemy->getTimeSinceLastAttack();
    const int spawnedBeforeNegative = manager.getEnemiesSpawnedInWave();
    manager.update(-1.0f);
    check(near(enemy->getPosition().x, beforeNegative),
          "Negative WaveManager delta does not move an existing enemy");
    check(near(enemy->getTimeSinceLastAttack(), attackTimeBeforeNegative),
          "Negative WaveManager delta does not change cooldown");
    check(manager.getEnemiesSpawnedInWave() == spawnedBeforeNegative,
          "Negative WaveManager delta does not spawn an enemy");
}

void checkProgressionAndVictory() {
    std::cout << "\n--- Wave transitions, variety, victory ---\n";
    WaveManager manager;
    manager.update(6.0f);
    check(manager.getActiveEnemyCount() == 2 && manager.getCurrentWave() == 1,
          "Wave 1 contains two enemies after 6 seconds");

    defeatAll(manager);
    manager.update(0.0f);
    check(manager.getCurrentWave() == 1 && !manager.hasWon(),
          "Killing all spawned enemies early does not skip remaining spawn quota");
    check(manager.getDropCount() == 2,
          "First two Wave 1 deaths generate two drops before wave completion");

    manager.update(3.0f);
    check(manager.getCurrentWave() == 1 && manager.getActiveEnemyCount() == 3,
          "Final Wave 1 enemy still spawns after earlier kills");
    manager.update(0.0f);
    check(manager.getCurrentWave() == 1, "Wave 1 waits for final living enemy");
    if (Enemy* last = manager.getEnemyAt(2)) last->takeDamage(1000);
    manager.update(0.0f);
    check(manager.getCurrentWave() == 2, "Wave 1 advances to Wave 2 once all are dead");
    check(manager.getDropCount() == 3,
          "All three Wave 1 drops survive the Wave 1-to-2 transition");
    check(manager.getActiveEnemyCount() == 0, "Previous wave enemies are cleared");
    check(manager.getEnemyAt(0) == nullptr, "Old wave indices do not remain valid");
    check(manager.getEnemiesSpawnedInWave() == 0, "Wave 2 spawn counter resets");
    check(!manager.hasWon(), "No premature victory at start of Wave 2");

    manager.update(0.0f);
    check(manager.getActiveEnemyCount() == 0, "No immediate spawn upon transition");
    check(manager.getDropCount() == 3,
          "Zero-time update in Wave 2 preserves previous drops");
    manager.update(2.5f);
    check(manager.getActiveEnemyCount() == 0, "Wave 2 spawn timer starts fresh");
    manager.update(0.5f);
    check(manager.getActiveEnemyCount() == 1 && isEasy(manager.getEnemyAt(0)),
          "First Wave 2 enemy is guaranteed Easy");
    manager.update(12.0f);
    check(manager.getEnemiesSpawnedInWave() == GameConfig::WAVE_2_COUNT &&
          manager.getActiveEnemyCount() == GameConfig::WAVE_2_COUNT,
          "Wave 2 spawns exactly five enemies");
    check(isMedium(manager.getEnemyAt(1)), "Second Wave 2 enemy is guaranteed Medium");

    bool wave2TypesAndLanesOK = true;
    for (std::size_t i = 0; i < manager.getActiveEnemyCount(); ++i) {
        wave2TypesAndLanesOK &= validTypeForWave(2, manager.getEnemyAt(i));
        const Enemy* e = manager.getEnemyAt(i);
        if (!e || e->getLane() < 0 || e->getLane() >= GameConfig::LANE_COUNT ||
            !near(e->getPosition().y, 100.0f + e->getLane() * 75.0f)) {
            wave2TypesAndLanesOK = false;
        }
    }
    check(wave2TypesAndLanesOK, "Every Wave 2 enemy has an allowed type and valid lane");
    check(manager.getDropCount() == 3,
          "Spawning Wave 2 enemies does not duplicate Wave 1 drops");

    // Killing four of five must not advance the wave.
    for (std::size_t i = 0; i < 4; ++i) {
        if (Enemy* e = manager.getEnemyAt(i)) e->takeDamage(1000);
    }
    manager.update(0.0f);
    check(manager.getCurrentWave() == 2 && !manager.hasWon(),
          "Wave 2 does not advance with one enemy still alive");
    check(manager.getDropCount() == 7,
          "Four Wave 2 deaths add exactly four drops");
    if (Enemy* last = manager.getEnemyAt(4)) last->takeDamage(1000);
    manager.update(0.0f);
    check(manager.getCurrentWave() == 3 && manager.getActiveEnemyCount() == 0,
          "Wave 2 advances to Wave 3 only after last death");
    check(manager.getDropCount() == 8,
          "All eight drops from Waves 1 and 2 survive Wave 3 transition");
    check(manager.getEnemiesSpawnedInWave() == 0 && !manager.hasWon(),
          "Wave 3 counters reset and victory is false");

    // Spawn nine of ten enemies; verify all three types before killing them.
    manager.update(27.0f);
    check(manager.getEnemiesSpawnedInWave() == 9 && manager.getActiveEnemyCount() == 9,
          "Nine enemies spawn before Wave 3's tenth spawn interval");
    check(isEasy(manager.getEnemyAt(0)) && isMedium(manager.getEnemyAt(1)) &&
          isHard(manager.getEnemyAt(2)),
          "Wave 3 guarantees Easy, Medium, and Hard as first three enemies");

    bool wave3TypesAndLanesOK = true;
    for (std::size_t i = 0; i < manager.getActiveEnemyCount(); ++i) {
        const Enemy* e = manager.getEnemyAt(i);
        wave3TypesAndLanesOK &= validTypeForWave(3, e);
        if (!e || e->getLane() < 0 || e->getLane() >= GameConfig::LANE_COUNT ||
            !near(e->getPosition().y, 100.0f + e->getLane() * 75.0f)) {
            wave3TypesAndLanesOK = false;
        }
    }
    check(wave3TypesAndLanesOK, "Every Wave 3 enemy has a valid type and lane");

    // First three are guaranteed Easy, Medium, Hard: verify each death drops once.
    const std::size_t dropsBeforeThreeTypes = manager.getDropCount();
    for (std::size_t i = 0; i < 3; ++i) {
        if (Enemy* e = manager.getEnemyAt(i)) e->takeDamage(1000);
    }
    manager.update(0.0f);
    check(manager.getDropCount() == dropsBeforeThreeTypes + 3,
          "Easy, Medium, Hard each generate exactly one collectible");
    manager.update(0.0f);
    check(manager.getDropCount() == dropsBeforeThreeTypes + 3,
          "Repeated updates do not duplicate drops from any enemy type");
    check(manager.getCurrentWave() == 3 && !manager.hasWon(),
          "Killing only first three Wave 3 enemies does not end game");

    defeatAll(manager);
    manager.update(0.0f);
    check(!manager.hasWon() && manager.getCurrentWave() == 3,
          "No victory until all ten Wave 3 enemies have spawned");
    check(manager.getDropCount() == 17,
          "Nine Wave 3 deaths yield 17 cumulative collectibles");
    manager.update(3.0f);
    check(manager.getActiveEnemyCount() == GameConfig::WAVE_3_COUNT &&
          manager.getEnemiesSpawnedInWave() == GameConfig::WAVE_3_COUNT,
          "Tenth Wave 3 enemy spawns at 30 seconds");
    check(!manager.hasWon(), "Spawning tenth living enemy does not trigger victory");
    check(manager.getDropCount() == 17,
          "Tenth living enemy has not generated its drop");

    if (Enemy* last = manager.getEnemyAt(9)) last->takeDamage(1000);
    manager.update(0.0f);
    check(manager.hasWon(), "All spawned and defeated Wave 3 enemies trigger victory");
    check(manager.getDropCount() == 18,
          "Final enemy's collectible is retained when victory occurs");
    check(manager.getDropAt(17) != nullptr,
          "Final victory collectible is accessible by index");
    check(manager.getCurrentWave() == 3, "Game never advances to Wave 4");

    const std::size_t finalCount = manager.getActiveEnemyCount();
    const int finalSpawns = manager.getEnemiesSpawnedInWave();
    manager.update(100.0f);
    manager.update(0.0f);
    check(manager.hasWon() && manager.getCurrentWave() == 3,
          "Victory remains true after future updates");
    check(manager.getActiveEnemyCount() == finalCount &&
          manager.getEnemiesSpawnedInWave() == finalSpawns,
          "No extra enemies spawn after victory");
    check(manager.getDropCount() == 18,
          "Updates after victory cannot duplicate any of the 18 drops");
}

void checkRepeatedFreshManagers() {
    std::cout << "\n--- Repeated independent sessions ---\n";
    bool good = true;
    for (int run = 0; run < 8; ++run) {
        WaveManager manager;
        good &= manager.getCurrentWave() == 1 && !manager.hasWon();
        manager.update(9.0f);
        good &= manager.getActiveEnemyCount() == 3;
        defeatAll(manager);
        manager.update(0.0f);
        good &= manager.getCurrentWave() == 2;
        good &= manager.getDropCount() == 3;
        manager.update(15.0f);
        good &= manager.getActiveEnemyCount() == 5;
        good &= isEasy(manager.getEnemyAt(0)) && isMedium(manager.getEnemyAt(1));
        defeatAll(manager);
        manager.update(0.0f);
        good &= manager.getCurrentWave() == 3;
        good &= manager.getDropCount() == 8;
        manager.update(30.0f);
        good &= manager.getActiveEnemyCount() == 10;
        good &= isEasy(manager.getEnemyAt(0)) && isMedium(manager.getEnemyAt(1)) &&
                isHard(manager.getEnemyAt(2));
        defeatAll(manager);
        manager.update(0.0f);
        good &= manager.hasWon();
        good &= manager.getDropCount() == 18;
    }
    check(good, "Eight independent complete games have correct waves, drops, victory");
}

void checkSingleDropLifecycle() {
    std::cout << "\n--- Collectible creation / pickup / invalid indices ---\n";
    WaveManager manager;
    manager.update(GameConfig::ENEMY_SPAWN_INTERVAL);
    check(manager.getActiveEnemyCount() == 1,
          "One enemy spawned for collectible testing");
    check(manager.getDropCount() == 0,
          "Living enemy has not generated a collectible");

    Enemy* enemy = manager.getEnemyAt(0);
    if (!enemy) {
        check(false, "Spawned enemy is accessible for drop tests");
        return;
    }

    const glm::vec2 expectedPosition = enemy->getPosition();
    const int expectedLane = enemy->getLane();
    enemy->takeDamage(1000);
    check(manager.getDropCount() == 0,
          "Death is processed by WaveManager.update, not immediately by takeDamage");
    manager.update(0.0f);
    check(manager.getDropCount() == 1, "Dead enemy generates one collectible");

    const EnemyDrop* drop = manager.getDropAt(0);
    check(drop && samePosition(drop->position, expectedPosition) &&
          drop->lane == expectedLane,
          "Collectible appears at dead enemy's position and lane");
    check(manager.getDropAt(1) == nullptr,
          "Index equal to drop count returns nullptr");
    check(manager.getDropAt(10) == nullptr,
          "Invalid collectible index returns nullptr");

    manager.update(0.0f);
    manager.update(0.0f);
    manager.update(0.5f);
    check(manager.getDropCount() == 1,
          "Repeated updates do not duplicate collectibles");

    const std::size_t enormous = std::numeric_limits<std::size_t>::max();
    check(manager.getDropAt(enormous) == nullptr,
          "Very large collectible index returns nullptr");
    check(!manager.removeDropAt(10), "Cannot remove nonexistent collectible");
    check(!manager.removeDropAt(enormous), "Very large removal index is rejected");
    check(manager.getDropCount() == 1,
          "Invalid removal does not change drop count");
    check(manager.removeDropAt(0), "Collectible is removed successfully");
    check(manager.getDropCount() == 0, "No collectibles remain after pickup");
    check(manager.getDropAt(0) == nullptr,
          "Collected drop cannot be accessed after removal");
    check(!manager.removeDropAt(0), "Same collectible cannot be collected twice");
    manager.update(0.0f);
    check(manager.getDropCount() == 0,
          "Already-collected drop is not regenerated by a dead enemy");
}

void checkMultipleDropsAndRemovalOrder() {
    std::cout << "\n--- Multiple deaths / removal order ---\n";
    WaveManager manager;
    manager.update(9.0f); // Wave 1: all three EasyEnemies spawn.
    check(manager.getActiveEnemyCount() == 3,
          "Three enemies available for multiple-drop test");
    if (manager.getActiveEnemyCount() != 3) return;

    const Enemy* first = manager.getEnemyAt(0);
    const Enemy* second = manager.getEnemyAt(1);
    const Enemy* third = manager.getEnemyAt(2);
    if (!first || !second || !third) {
        check(false, "All three enemies accessible for removal-order test");
        return;
    }
    const glm::vec2 firstPosition = first->getPosition();
    const glm::vec2 secondPosition = second->getPosition();
    const glm::vec2 thirdPosition = third->getPosition();
    const int firstLane = first->getLane();
    const int secondLane = second->getLane();
    const int thirdLane = third->getLane();

    // Process deaths one at a time so drop order is deterministic.
    manager.getEnemyAt(0)->takeDamage(1000);
    manager.update(0.0f);
    check(manager.getDropCount() == 1,
          "First enemy death produces first drop");
    check(manager.getCurrentWave() == 1,
          "Wave remains active while two enemies are alive");

    manager.getEnemyAt(1)->takeDamage(1000);
    manager.update(0.0f);
    check(manager.getDropCount() == 2,
          "Second enemy death produces second drop");

    manager.getEnemyAt(2)->takeDamage(1000);
    manager.update(0.0f);
    check(manager.getCurrentWave() == 2,
          "Wave advances when last of three enemies dies");
    check(manager.getDropCount() == 3,
          "Last enemy's drop survives when active enemies are cleared");

    const EnemyDrop* drop0 = manager.getDropAt(0);
    const EnemyDrop* drop1 = manager.getDropAt(1);
    const EnemyDrop* drop2 = manager.getDropAt(2);
    check(drop0 && samePosition(drop0->position, firstPosition) && drop0->lane == firstLane,
          "First drop preserves first enemy position and lane");
    check(drop1 && samePosition(drop1->position, secondPosition) && drop1->lane == secondLane,
          "Second drop preserves second enemy position and lane");
    check(drop2 && samePosition(drop2->position, thirdPosition) && drop2->lane == thirdLane,
          "Third drop preserves third enemy position and lane");

    check(manager.removeDropAt(1), "Removing middle drop succeeds");
    check(manager.getDropCount() == 2, "Middle removal reduces drop count to two");
    const EnemyDrop* newFirst = manager.getDropAt(0);
    const EnemyDrop* newSecond = manager.getDropAt(1);
    check(newFirst && samePosition(newFirst->position, firstPosition) &&
          newFirst->lane == firstLane,
          "Middle removal preserves the earlier drop");
    check(newSecond && samePosition(newSecond->position, thirdPosition) &&
          newSecond->lane == thirdLane,
          "Middle removal shifts the later drop into the removed index");
    check(manager.removeDropAt(1), "Removing last remaining index succeeds");
    check(manager.removeDropAt(0), "Removing first remaining index succeeds");
    check(manager.getDropCount() == 0, "Removing all drops leaves an empty list");
    manager.update(0.0f);
    check(manager.getDropCount() == 0,
          "Previously collected drops stay removed across a wave transition");
}

void checkNegativeTimeDefersDropProcessing() {
    std::cout << "\n--- Death processing boundaries ---\n";
    WaveManager manager;
    manager.update(3.0f);
    Enemy* enemy = manager.getEnemyAt(0);
    if (!enemy) {
        check(false, "Enemy available for negative-time drop test");
        return;
    }
    enemy->takeDamage(1000);
    manager.update(-1.0f);
    check(manager.getDropCount() == 0,
          "Negative WaveManager update does not process a pending death");
    manager.update(0.0f);
    check(manager.getDropCount() == 1,
          "Zero-time update processes a previously pending death");
    manager.update(0.0f);
    check(manager.getDropCount() == 1,
          "Pending death is recorded only once");
}

void checkDropIndependenceBetweenManagers() {
    std::cout << "\n--- Independent WaveManager drop storage ---\n";
    WaveManager managerA;
    WaveManager managerB;
    managerA.update(3.0f);
    managerB.update(3.0f);
    if (Enemy* e = managerA.getEnemyAt(0)) e->takeDamage(1000);
    managerA.update(0.0f);
    check(managerA.getDropCount() == 1,
          "First WaveManager records its enemy's drop");
    check(managerB.getDropCount() == 0,
          "Separate WaveManager has no drops from first manager");
    check(managerA.removeDropAt(0),
          "First WaveManager can remove its own drop");
    check(managerB.getDropCount() == 0,
          "Removing drops in first WaveManager does not affect another");
}

} // namespace

int main() {
    checkInitialState();
    checkTimeBoundaries();
    checkMovementAndStates();
    checkProgressionAndVictory();
    checkRepeatedFreshManagers();
    checkSingleDropLifecycle();
    checkMultipleDropsAndRemovalOrder();
    checkNegativeTimeDefersDropProcessing();
    checkDropIndependenceBetweenManagers();

    std::cout << "\n==============================\n";
    std::cout << "WaveManager tests: " << passed << " passed, "
              << failed << " failed\n";
    std::cout << "==============================\n";
    return failed == 0 ? 0 : 1;
}