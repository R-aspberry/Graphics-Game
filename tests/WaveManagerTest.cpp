
#include "../src/systems/WaveManager.h"
#include <cassert>
#include <iostream>
#include <cmath>
int main() {

    // TEST 1: Initially no enemies have spawned
    WaveManager wave;

    assert(wave.getEnemiesSpawnedInWave() == 0);
    assert(wave.getActiveEnemyCount() == 0);

    // TEST 2: Not enough time has passed
    wave.update(2.0f);

    assert(wave.getEnemiesSpawnedInWave() == 0);
    assert(wave.getActiveEnemyCount() == 0);

    // TEST 3: First enemy spawns at 3 seconds
    wave.update(1.0f);

    assert(wave.getEnemiesSpawnedInWave() == 1);
    assert(wave.getActiveEnemyCount() == 1);

    // TEST 4: Second enemy spawns at 6 seconds
    wave.update(3.0f);

    assert(wave.getEnemiesSpawnedInWave() == 2);
    assert(wave.getActiveEnemyCount() == 2);

    // TEST 5: Third enemy spawns at 9 seconds
    wave.update(3.0f);

    assert(wave.getEnemiesSpawnedInWave() == 3);
    assert(wave.getActiveEnemyCount() == 3);

    // TEST 6: Wave 1 cannot spawn more than 3 enemies
    wave.update(10.0f);

    assert(wave.getEnemiesSpawnedInWave() == 3);
    assert(wave.getActiveEnemyCount() == 3);

    // TEST 7: A large deltaTime spawns multiple enemies
    WaveManager secondWaveTest;
    secondWaveTest.update(7.0f);

    assert(secondWaveTest.getEnemiesSpawnedInWave() == 2);
    assert(secondWaveTest.getActiveEnemyCount() == 2);

    // TEST 8: Negative deltaTime is ignored
    WaveManager negativeTimeTest;
    negativeTimeTest.update(-5.0f);

    assert(negativeTimeTest.getEnemiesSpawnedInWave() == 0);
    assert(negativeTimeTest.getActiveEnemyCount() == 0);

    std::cout << "All Wave 1 spawning tests passed!\n";
    return 0;

    
    // TEST 9: Cannot access an enemy before spawning
    WaveManager movementTest;

    assert(movementTest.getEnemyAt(0) == nullptr);
    std::cout << "[PASS] No enemy exists before spawning\n";

    // TEST 10: First enemy spawns at the correct position
    movementTest.update(3.0f);

    const Enemy* firstEnemy = movementTest.getEnemyAt(0);

    assert(firstEnemy != nullptr);
    assert(firstEnemy->getPosition().x == 900.0f);
    std::cout << "[PASS] Enemy starts at X = 900\n";

    // TEST 11: Accessing an invalid index returns nullptr
    assert(movementTest.getEnemyAt(1) == nullptr);
    std::cout << "[PASS] Invalid enemy index returns nullptr\n";

    // TEST 12: Enemy moves left after 0.5 seconds
    float initialY = firstEnemy->getPosition().y;

    movementTest.update(0.5f);

    assert(std::fabs(firstEnemy->getPosition().x - 870.0f) < 0.001f);
    std::cout << "[PASS] Enemy moves from X = 900 to X = 870\n";

    // TEST 13: Enemy stays in the same lane
    assert(std::fabs(firstEnemy->getPosition().y - initialY) < 0.001f);
    std::cout << "[PASS] Enemy stays in the same lane\n";

    // TEST 14: Negative deltaTime does not move the enemy
    movementTest.update(-1.0f);

    assert(std::fabs(firstEnemy->getPosition().x - 870.0f) < 0.001f);
    std::cout << "[PASS] Negative deltaTime does not move the enemy\n";

}
