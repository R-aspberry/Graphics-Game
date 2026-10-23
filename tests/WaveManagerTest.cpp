#include "../src/systems/WaveManager.h"
#include "../src/entities/EasyEnemy.h"
#include "../src/entities/MediumEnemy.h"
#include "../src/entities/HardEnemy.h"
#include "../src/utils/Constants.h"

#include <cmath>
#include <cstddef>
#include <iostream>
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

bool validLaneAndSpawn(const Enemy* enemy) {
    if (!enemy) return false;
    const int lane = enemy->getLane();
    return lane >= 0 && lane < GameConfig::LANE_COUNT &&
           near(enemy->getPosition().x, 900.0f) &&
           near(enemy->getPosition().y, 100.0f + lane * 75.0f);
}

bool isEasy(const Enemy* e) { return dynamic_cast<const EasyEnemy*>(e) != nullptr; }
bool isMedium(const Enemy* e) { return dynamic_cast<const MediumEnemy*>(e) != nullptr; }
bool isHard(const Enemy* e) { return dynamic_cast<const HardEnemy*>(e) != nullptr; }

bool validTypeForWave(int wave, const Enemy* enemy) {
    if (!enemy) return false;
    if (wave == 1) return isEasy(enemy);
    if (wave == 2) return isEasy(enemy) || isMedium(enemy);
    if (wave == 3) return isEasy(enemy) || isMedium(enemy) || isHard(enemy);
    return false;
}

void defeatAll(WaveManager& manager) {
    // Do not erase enemies while iterating: WaveManager owns them.
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

    // Verifies both const and non-const getEnemyAt overloads are available.
    static_assert(std::is_same<decltype(std::declval<WaveManager&>().getEnemyAt(0)), Enemy*>::value,
                  "Non-const WaveManager must provide mutable Enemy* getter");
    static_assert(std::is_same<decltype(std::declval<const WaveManager&>().getEnemyAt(0)), const Enemy*>::value,
                  "Const WaveManager must provide const Enemy* getter");
    const WaveManager& readOnly = manager;
    check(readOnly.getEnemyAt(0) == nullptr, "Const getter handles invalid index");
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
    if (first) {
        check(near(first->getTimeSinceLastAttack(), 0.0f),
              "Newly spawned enemy's attack timer starts at zero");
    }
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

    WaveManager largeFrame;
    largeFrame.update(9.0f);
    check(largeFrame.getActiveEnemyCount() == 3, "One 9-second update spawns three enemies");
    bool allStartAtEdge = true;
    for (std::size_t i = 0; i < largeFrame.getActiveEnemyCount(); ++i) {
        allStartAtEdge &= validLaneAndSpawn(largeFrame.getEnemyAt(i));
    }
    check(allStartAtEdge, "All new enemies start at spawn edge in same large frame");
    largeFrame.update(0.0f);
    check(largeFrame.getActiveEnemyCount() == 3, "Zero-time update does not duplicate spawns");
}

void checkMovementAndStates() {
    std::cout << "\n--- Movement / negative delta / state ---\n";
    WaveManager manager;
    manager.update(3.0f);
    Enemy* enemy = manager.getEnemyAt(0);
    check(enemy != nullptr, "Can access existing enemy by mutable getter");
    if (!enemy) return;
    const float laneY = enemy->getPosition().y;
    manager.update(0.5f);
    check(near(enemy->getPosition().x, 870.0f), "EasyEnemy moves left 30 units in 0.5 s");
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
    manager.update(3.0f);
    check(manager.getCurrentWave() == 1 && manager.getActiveEnemyCount() == 3,
          "Final Wave 1 enemy still spawns after earlier kills");
    manager.update(0.0f);
    check(manager.getCurrentWave() == 1, "Wave 1 waits for final living enemy");
    if (Enemy* last = manager.getEnemyAt(2)) last->takeDamage(1000);
    manager.update(0.0f);
    check(manager.getCurrentWave() == 2, "Wave 1 advances to Wave 2 once all are dead");
    check(manager.getActiveEnemyCount() == 0, "Previous wave enemies are cleared");
    check(manager.getEnemyAt(0) == nullptr, "Old wave indices do not remain valid");
    check(manager.getEnemiesSpawnedInWave() == 0, "Wave 2 spawn counter resets");
    check(!manager.hasWon(), "No premature victory at start of Wave 2");
    manager.update(0.0f);
    check(manager.getActiveEnemyCount() == 0, "No immediate spawn upon transition");
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
    // Killing four of five must not advance the wave.
    for (std::size_t i = 0; i < 4; ++i) {
        if (Enemy* e = manager.getEnemyAt(i)) e->takeDamage(1000);
    }
    manager.update(0.0f);
    check(manager.getCurrentWave() == 2 && !manager.hasWon(),
          "Wave 2 does not advance with one enemy still alive");
    if (Enemy* last = manager.getEnemyAt(4)) last->takeDamage(1000);
    manager.update(0.0f);
    check(manager.getCurrentWave() == 3 && manager.getActiveEnemyCount() == 0,
          "Wave 2 advances to Wave 3 only after last death");
    check(manager.getEnemiesSpawnedInWave() == 0 && !manager.hasWon(),
          "Wave 3 counters reset and victory is false");

    // Spawn only nine of the ten enemies, then kill them all.
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
    defeatAll(manager);
    manager.update(0.0f);
    check(!manager.hasWon() && manager.getCurrentWave() == 3,
          "No victory until all ten Wave 3 enemies have spawned");
    manager.update(3.0f);
    check(manager.getActiveEnemyCount() == GameConfig::WAVE_3_COUNT &&
          manager.getEnemiesSpawnedInWave() == GameConfig::WAVE_3_COUNT,
          "Tenth Wave 3 enemy spawns at 30 seconds");
    check(!manager.hasWon(), "Spawning tenth living enemy does not trigger victory");
    if (Enemy* last = manager.getEnemyAt(9)) last->takeDamage(1000);
    manager.update(0.0f);
    check(manager.hasWon(), "All spawned and defeated Wave 3 enemies trigger victory");
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
        manager.update(15.0f);
        good &= manager.getActiveEnemyCount() == 5;
        good &= isEasy(manager.getEnemyAt(0)) && isMedium(manager.getEnemyAt(1));
        defeatAll(manager);
        manager.update(0.0f);
        good &= manager.getCurrentWave() == 3;
        manager.update(30.0f);
        good &= manager.getActiveEnemyCount() == 10;
        good &= isEasy(manager.getEnemyAt(0)) && isMedium(manager.getEnemyAt(1)) &&
                isHard(manager.getEnemyAt(2));
        defeatAll(manager);
        manager.update(0.0f);
        good &= manager.hasWon();
    }
    check(good, "Eight independent complete games have correct waves and victory");
}
} // namespace

int main() {
    checkInitialState();
    checkTimeBoundaries();
    checkMovementAndStates();
    checkProgressionAndVictory();
    checkRepeatedFreshManagers();
    std::cout << "\nWaveManager tests: " << passed << " passed, "
              << failed << " failed\n";
    return failed == 0 ? 0 : 1;
}
