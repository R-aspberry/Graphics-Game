
#include <iostream>
#include <cmath>
#include <string>
#include "Enemy.h"

int passed = 0;
int failed = 0;

// Prints whether each test passed or failed
void check(bool condition, const std::string& testName) {
    if (condition) {
        std::cout << "[PASS] " << testName << '\n';
        passed++;
    } else {
        std::cout << "[FAIL] " << testName << '\n';
        failed++;
    }
}

// Compare decimal numbers safely
bool approximatelyEqual(float a, float b) {
    return std::fabs(a - b) < 0.001f;
}

int main() {

    std::cout << "===== ENEMY CLASS TESTS =====\n\n";

    // -----------------------------------------
    // TEST 1: Constructor and starting values
    // -----------------------------------------

    Enemy enemy(5, 1, 60.0f, 2.0f, 2,
                glm::vec2(900.0f, 250.0f));

    check(enemy.getHealth() == 5,
          "Initial health is 5");

    check(enemy.getMaxHealth() == 5,
          "Maximum health is 5");

    check(enemy.getDamage() == 1,
          "Damage is 1");

    check(approximatelyEqual(enemy.getSpeed(), 60.0f),
          "Speed is 60");

    check(enemy.getLane() == 2,
          "Enemy starts in lane 2");

    check(enemy.getState() == EnemyState::MOVING,
          "Enemy starts in MOVING state");

    check(approximatelyEqual(enemy.getPosition().x, 900.0f),
          "Starting X is 900");

    check(approximatelyEqual(enemy.getPosition().y, 250.0f),
          "Starting Y is 250");

    // -----------------------------------------
    // TEST 2: Movement
    // -----------------------------------------

    std::cout << "\n--- Movement Tests ---\n";

    enemy.update(0.5f);

    check(approximatelyEqual(enemy.getPosition().x, 870.0f),
          "After 0.5 seconds, X is 870");

    enemy.update(0.5f);

    check(approximatelyEqual(enemy.getPosition().x, 840.0f),
          "After 1 total second, X is 840");

    check(approximatelyEqual(enemy.getPosition().y, 250.0f),
          "Y does not change during movement");

    // -----------------------------------------
    // TEST 3: Fighting state
    // -----------------------------------------

    std::cout << "\n--- State Tests ---\n";

    enemy.setState(EnemyState::FIGHTING);

    check(enemy.getState() == EnemyState::FIGHTING,
          "Enemy changes to FIGHTING");

    enemy.update(1.0f);

    check(approximatelyEqual(enemy.getPosition().x, 840.0f),
          "Enemy does not move while FIGHTING");

    // -----------------------------------------
    // TEST 4: Attack cooldown
    // -----------------------------------------

    std::cout << "\n--- Attack Timer Tests ---\n";

    // Total elapsed time: 0.5 + 0.5 + 1.0 = 2.0
    check(enemy.canAttack(),
          "Enemy can attack after 2 seconds");

    enemy.resetAttackTimer();

    check(!enemy.canAttack(),
          "Enemy cannot attack immediately after reset");

    enemy.update(0.5f);

    check(!enemy.canAttack(),
          "Enemy cannot attack after only 0.5 seconds");

    check(approximatelyEqual(enemy.getTimeSinceLastAttack(), 0.5f),
          "Attack timer increased to 0.5");

    enemy.update(1.5f);

    check(enemy.canAttack(),
          "Enemy can attack when cooldown is complete");

    // -----------------------------------------
    // TEST 5: Return to movement
    // -----------------------------------------

    std::cout << "\n--- Resume Movement Test ---\n";

    enemy.setState(EnemyState::MOVING);
    enemy.update(1.0f);

    check(approximatelyEqual(enemy.getPosition().x, 780.0f),
          "Enemy resumes moving after FIGHTING");

    // -----------------------------------------
    // TEST 6: Taking damage
    // -----------------------------------------

    std::cout << "\n--- Damage Tests ---\n";

    enemy.takeDamage(2);

    check(enemy.getHealth() == 3,
          "Taking 2 damage reduces health to 3");

    enemy.takeDamage(-5);

    check(enemy.getHealth() == 3,
          "Negative damage is ignored");

    enemy.takeDamage(10);

    check(enemy.getHealth() == 0,
          "Health cannot become negative");

    check(enemy.getState() == EnemyState::DEAD,
          "Enemy enters DEAD state at zero health");

    // -----------------------------------------
    // TEST 7: Dead enemies
    // -----------------------------------------

    std::cout << "\n--- Death Tests ---\n";

    float previousX = enemy.getPosition().x;
    float previousTimer = enemy.getTimeSinceLastAttack();

    enemy.update(2.0f);

    check(approximatelyEqual(enemy.getPosition().x, previousX),
          "Dead enemy cannot move");

    check(approximatelyEqual(
              enemy.getTimeSinceLastAttack(), previousTimer),
          "Dead enemy's attack timer stops");

    check(!enemy.canAttack(),
          "Dead enemy cannot attack");

    enemy.setState(EnemyState::MOVING);

    check(enemy.getState() == EnemyState::DEAD,
          "Dead enemy cannot return to MOVING");

    enemy.takeDamage(5);

    check(enemy.getHealth() == 0,
          "Dead enemy ignores further damage");

    // -----------------------------------------
    // TEST 8: Prevent unauthorized death
    // -----------------------------------------

    std::cout << "\n--- State Protection Test ---\n";

    Enemy healthyEnemy(5, 1, 60.0f, 2.0f, 0,
                       glm::vec2(900.0f, 100.0f));

    healthyEnemy.setState(EnemyState::DEAD);

    check(healthyEnemy.getState() == EnemyState::MOVING,
          "setState cannot kill a healthy enemy");

    check(healthyEnemy.getHealth() == 5,
          "Healthy enemy retains its health");

    // -----------------------------------------
    // FINAL RESULTS
    // -----------------------------------------

    std::cout << "\n==============================\n";
    std::cout << "PASSED: " << passed << '\n';
    std::cout << "FAILED: " << failed << '\n';
    std::cout << "==============================\n";

    if (failed == 0) {
        std::cout << "ALL ENEMY TESTS PASSED!\n";
        return 0;
    }

    return 1;
}
