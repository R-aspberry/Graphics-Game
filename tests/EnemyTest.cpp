
#include <iostream>
#include <cmath>
#include <string>
#include "Enemy.h"
#include "EasyEnemy.h"
#include "MediumEnemy.h"
#include "HardEnemy.h"

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
    // TEST 9: EasyEnemy (Sword Fighter)
    // -----------------------------------------

    std::cout << "\n--- EasyEnemy Tests ---\n";

    EasyEnemy swordFighter(
        2,
        glm::vec2(900.0f, 250.0f),
        2.0f
    );

    // Test initial properties
    check(swordFighter.getHealth() == 5,
          "EasyEnemy starts with 5 HP");

    check(swordFighter.getMaxHealth() == 5,
          "EasyEnemy maximum health is 5");

    check(swordFighter.getDamage() == 1,
          "EasyEnemy deals 1 damage");

    check(approximatelyEqual(swordFighter.getSpeed(), 60.0f),
          "EasyEnemy speed is 60");

    check(swordFighter.getLane() == 2,
          "EasyEnemy spawns in the correct lane");

    check(swordFighter.getState() == EnemyState::MOVING,
          "EasyEnemy starts in MOVING state");

    check(approximatelyEqual(swordFighter.getAttackInterval(), 2.0f),
          "EasyEnemy uses the supplied attack interval");

    // Test inherited movement
    swordFighter.update(0.5f);

    check(approximatelyEqual(swordFighter.getPosition().x, 870.0f),
          "EasyEnemy inherits movement correctly");

    check(approximatelyEqual(swordFighter.getPosition().y, 250.0f),
          "EasyEnemy stays in its lane");

    // Test inherited damage
    swordFighter.takeDamage(2);

    check(swordFighter.getHealth() == 3,
          "EasyEnemy inherits damage handling");

    // Test inherited death behavior
    swordFighter.takeDamage(5);

    check(swordFighter.getHealth() == 0,
          "EasyEnemy health reaches zero");

    check(swordFighter.getState() == EnemyState::DEAD,
          "EasyEnemy dies correctly");

    check(!swordFighter.canAttack(),
          "Dead EasyEnemy cannot attack");


    
    // -----------------------------------------
    // TEST 10: MediumEnemy (Archer)
    // -----------------------------------------

    std::cout << "\n--- MediumEnemy Tests ---\n";

    MediumEnemy archer(
        1,
        glm::vec2(900.0f, 175.0f),
        1.5f
    );

    // Test initial properties
    check(archer.getHealth() == 5,
          "MediumEnemy starts with 5 HP");

    check(archer.getMaxHealth() == 5,
          "MediumEnemy maximum health is 5");

    check(archer.getDamage() == 1,
          "MediumEnemy deals 1 damage");

    check(approximatelyEqual(archer.getSpeed(), 60.0f),
          "MediumEnemy speed is 60");

    check(archer.getLane() == 1,
          "MediumEnemy spawns in the correct lane");

    check(archer.getState() == EnemyState::MOVING,
          "MediumEnemy starts in MOVING state");

    check(approximatelyEqual(archer.getAttackInterval(), 1.5f),
          "MediumEnemy uses the supplied attack interval");

    // Test inherited movement
    archer.update(0.5f);

    check(approximatelyEqual(archer.getPosition().x, 870.0f),
          "MediumEnemy inherits movement correctly");

    check(approximatelyEqual(archer.getPosition().y, 175.0f),
          "MediumEnemy stays in its lane");

    // Test inherited damage
    archer.takeDamage(2);

    check(archer.getHealth() == 3,
          "MediumEnemy inherits damage handling");

    // Test inherited death behavior
    archer.takeDamage(5);

    check(archer.getHealth() == 0,
          "MediumEnemy health reaches zero");

    check(archer.getState() == EnemyState::DEAD,
          "MediumEnemy dies correctly");

    check(!archer.canAttack(),
          "Dead MediumEnemy cannot attack");

      
    // -----------------------------------------
    // TEST 11: HardEnemy (Armored Knight)
    // -----------------------------------------

    std::cout << "\n--- HardEnemy Tests ---\n";

    HardEnemy knight(
        3,
        glm::vec2(900.0f, 325.0f),
        2.0f
    );

    check(knight.getHealth() == 7,
          "HardEnemy starts with 7 HP");

    check(knight.getMaxHealth() == 7,
          "HardEnemy maximum health is 7");

    check(knight.getDamage() == 2,
          "HardEnemy deals 2 damage");

    check(approximatelyEqual(knight.getSpeed(), 110.0f),
          "HardEnemy speed is 110");

    check(knight.getLane() == 3,
          "HardEnemy spawns in the correct lane");

    check(knight.getState() == EnemyState::MOVING,
          "HardEnemy starts in MOVING state");

    check(approximatelyEqual(knight.getAttackInterval(), 2.0f),
          "HardEnemy uses the supplied attack interval");

    // Inherited movement
    knight.update(0.5f);

    check(approximatelyEqual(knight.getPosition().x, 845.0f),
          "HardEnemy moves 55 units in 0.5 seconds");

    check(approximatelyEqual(knight.getPosition().y, 325.0f),
          "HardEnemy stays in its lane");

    // Inherited damage handling
    knight.takeDamage(2);

    check(knight.getHealth() == 5,
          "HardEnemy health decreases from 7 to 5");

    // Inherited death behavior
    knight.takeDamage(10);

    check(knight.getHealth() == 0,
          "HardEnemy health cannot become negative");

    check(knight.getState() == EnemyState::DEAD,
          "HardEnemy enters DEAD state");

    check(!knight.canAttack(),
          "Dead HardEnemy cannot attack");

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
