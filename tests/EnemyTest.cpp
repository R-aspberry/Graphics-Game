
#include <iostream>
#include <cmath>
#include <string>
#include <memory>
#include <limits>
#include "../src/utils/Constants.h"
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
    // ADDITIONAL BOUNDARY AND EDGE CASES
    // -----------------------------------------
    
std::cout << "\n--- Initial Values and Zero Time ---\n";

Enemy initial(5, 2, 60.0f, 2.0f, 3,
              glm::vec2(900.0f, 325.0f));

check(approximatelyEqual(initial.getTimeSinceLastAttack(),
                         initial.getAttackInterval()),
      "Attack timer starts at the cooldown threshold");

check(approximatelyEqual(initial.getAttackInterval(), 2.0f),
      "Base enemy preserves its attack interval");

check(initial.canAttack(),
      "Fresh enemy can attack immediately");

float timerBefore = initial.getTimeSinceLastAttack();

initial.update(0.0f);

check(approximatelyEqual(initial.getPosition().x, 900.0f),
      "Zero deltaTime does not move enemy");

check(approximatelyEqual(initial.getTimeSinceLastAttack(), timerBefore),
      "Zero deltaTime does not advance cooldown");

    std::cout << "\n--- Cooldown Thresholds ---\n";
    Enemy cooldown(5, 1, 60.0f, 2.0f, 0,
               glm::vec2(900.0f, 100.0f));

// Simulate the first attack, starting the next cooldown
cooldown.resetAttackTimer();

cooldown.update(1.5f);
    check(!cooldown.canAttack(), "Cannot attack strictly before cooldown ends");
    cooldown.update(0.5f);
    check(cooldown.canAttack(), "Can attack exactly at cooldown threshold");
    check(approximatelyEqual(cooldown.getTimeSinceLastAttack(), 2.0f),
          "Cooldown timer is exactly 2 seconds at threshold");
    cooldown.update(4.0f);
    check(cooldown.canAttack(), "Cooldown remains ready until manually reset");
    check(approximatelyEqual(cooldown.getTimeSinceLastAttack(), 6.0f),
          "Cooldown continues increasing beyond threshold");
    cooldown.resetAttackTimer();
    check(!cooldown.canAttack(), "Reset makes attack unavailable again");
    check(approximatelyEqual(cooldown.getTimeSinceLastAttack(), 0.0f),
          "Reset sets cooldown timer to zero");
    cooldown.setState(EnemyState::FIGHTING);
    const float beforeFight = cooldown.getPosition().x;
    cooldown.update(2.0f);
    check(cooldown.canAttack(), "Cooldown runs during FIGHTING state");
    check(approximatelyEqual(cooldown.getPosition().x, beforeFight),
          "FIGHTING state stops movement");
    cooldown.setState(EnemyState::MOVING);
    cooldown.update(0.25f);
    check(approximatelyEqual(cooldown.getPosition().x, beforeFight - 15.0f),
          "Movement resumes and handles fractional deltaTime");

    std::cout << "\n--- Damage Boundaries and Invalid States ---\n";
    Enemy damageBoundary(7, 2, 110.0f, 1.0f, 1, glm::vec2(900.0f, 175.0f));
    damageBoundary.takeDamage(0);
    check(damageBoundary.getHealth() == 7, "Zero damage is ignored");
    damageBoundary.takeDamage(-1);
    check(damageBoundary.getHealth() == 7, "Negative damage is ignored");
    damageBoundary.takeDamage(6);
    check(damageBoundary.getHealth() == 1 &&
          damageBoundary.getState() != EnemyState::DEAD,
          "Damage leaving exactly 1 HP does not kill");
    damageBoundary.takeDamage(1);
    check(damageBoundary.getHealth() == 0 &&
          damageBoundary.getState() == EnemyState::DEAD,
          "Exact lethal damage clamps to zero and kills");
    damageBoundary.takeDamage(std::numeric_limits<int>::max());
    check(damageBoundary.getHealth() == 0,
          "Additional damage to dead enemy is ignored");
    damageBoundary.setState(EnemyState::FIGHTING);
    check(damageBoundary.getState() == EnemyState::DEAD,
          "Dead enemy cannot re-enter FIGHTING");
    damageBoundary.resetAttackTimer();
    check(!damageBoundary.canAttack(), "Dead enemy cannot attack after timer reset");

    Enemy livingState(5, 1, 60.0f, 2.0f, 0, glm::vec2(800.0f, 100.0f));
    livingState.setState(EnemyState::FIGHTING);
    livingState.setState(EnemyState::DEAD);
    check(livingState.getState() == EnemyState::FIGHTING,
          "setState(DEAD) cannot kill from FIGHTING without damage");
    livingState.setState(EnemyState::MOVING);
    check(livingState.getState() == EnemyState::MOVING,
          "Living enemy may return to MOVING");
    livingState.takeDamage(std::numeric_limits<int>::max());
    check(livingState.getHealth() == 0 &&
          livingState.getState() == EnemyState::DEAD,
          "Large overkill damage is clamped to zero");

    std::cout << "\n--- Config, Inheritance, and Ownership ---\n";
    EasyEnemy extraEasy(0, glm::vec2(900.0f, 100.0f), 0.75f);
    MediumEnemy extraMedium(1, glm::vec2(900.0f, 175.0f), 1.25f);
    HardEnemy extraHard(3, glm::vec2(900.0f, 325.0f), 3.0f);
    check(extraEasy.getHealth() == GameConfig::EASY_ENEMY_HP &&
          extraEasy.getDamage() == GameConfig::EASY_ENEMY_DAMAGE &&
          approximatelyEqual(extraEasy.getSpeed(), GameConfig::EASY_ENEMY_SPEED),
          "EasyEnemy initial stats follow GameConfig");
    check(extraMedium.getHealth() == GameConfig::MEDIUM_ENEMY_HP &&
          extraMedium.getDamage() == GameConfig::MEDIUM_ENEMY_DAMAGE &&
          approximatelyEqual(extraMedium.getSpeed(), GameConfig::MEDIUM_ENEMY_SPEED),
          "MediumEnemy initial stats follow GameConfig");
    check(extraHard.getHealth() == GameConfig::HARD_ENEMY_HP &&
          extraHard.getDamage() == GameConfig::HARD_ENEMY_DAMAGE &&
          approximatelyEqual(extraHard.getSpeed(), GameConfig::HARD_ENEMY_SPEED),
          "HardEnemy initial stats follow GameConfig");
    check(approximatelyEqual(extraEasy.getAttackInterval(), 0.75f) &&
          approximatelyEqual(extraMedium.getAttackInterval(), 1.25f) &&
          approximatelyEqual(extraHard.getAttackInterval(), 3.0f),
          "Each subclass retains its own supplied cooldown");
    check(extraEasy.getLane() == 0 && extraMedium.getLane() == 1 &&
          extraHard.getLane() == 3,
          "Subclass constructors preserve valid lane indices");

    std::unique_ptr<Enemy> polymorphic =
        std::make_unique<HardEnemy>(2, glm::vec2(900.0f, 250.0f), 2.0f);
    check(dynamic_cast<HardEnemy*>(polymorphic.get()) != nullptr,
          "Parent-class unique_ptr preserves the actual child type");
    polymorphic->update(1.0f);
    check(approximatelyEqual(polymorphic->getPosition().x,
                             900.0f - GameConfig::HARD_ENEMY_SPEED),
          "Parent-class unique_ptr can update subclass movement");
    polymorphic->takeDamage(GameConfig::HARD_ENEMY_HP);
    check(polymorphic->getState() == EnemyState::DEAD,
          "Parent-class unique_ptr can kill subclass enemy");

    struct DestructionProbe : Enemy {
        bool& wasDestroyed;
        explicit DestructionProbe(bool& flag)
            : Enemy(1, 1, 1.0f, 1.0f, 0, glm::vec2(0.0f)),
              wasDestroyed(flag) {}
        ~DestructionProbe() override { wasDestroyed = true; }
    };
    bool derivedDestructorCalled = false;
    {
        std::unique_ptr<Enemy> pointer =
            std::make_unique<DestructionProbe>(derivedDestructorCalled);
    }
    check(derivedDestructorCalled,
          "Virtual destructor calls derived destructor through Enemy pointer");

   
std::cout << "\n--- Negative deltaTime Regression ---\n";

Enemy negativeTime(5, 1, 60.0f, 2.0f, 0,
                   glm::vec2(900.0f, 100.0f));

float timerBeforeNegative = negativeTime.getTimeSinceLastAttack();

negativeTime.update(-1.0f);

check(approximatelyEqual(negativeTime.getPosition().x, 900.0f),
      "Negative deltaTime must not move enemy backwards");

check(approximatelyEqual(negativeTime.getTimeSinceLastAttack(),
                         timerBeforeNegative),
      "Negative deltaTime must not reduce attack timer");

    // -----------------------------------------
    // TEST: Virtual Attack Types
    // -----------------------------------------

    std::cout << "\n--- Enemy Attack Type Tests ---\n";

    EasyEnemy attackEasy(0, glm::vec2(900.0f, 100.0f), 2.0f);
    MediumEnemy attackMedium(1, glm::vec2(900.0f, 175.0f), 2.0f);
    HardEnemy attackHard(2, glm::vec2(900.0f, 250.0f), 2.0f);

    // Test direct subclass calls
    check(attackEasy.getAttackType() == AttackType::High,
          "EasyEnemy uses high projectile attacks");

    check(attackMedium.getAttackType() == AttackType::Low,
          "MediumEnemy uses low projectile attacks");

    check(attackHard.getAttackType() == AttackType::Melee,
          "HardEnemy uses melee attacks");

    // Test virtual dispatch through parent pointers
    const Enemy* easyPtr = &attackEasy;
    const Enemy* mediumPtr = &attackMedium;
    const Enemy* hardPtr = &attackHard;

    check(easyPtr->getAttackType() == AttackType::High,
          "EasyEnemy virtual dispatch works");

    check(mediumPtr->getAttackType() == AttackType::Low,
          "MediumEnemy virtual dispatch works");

    check(hardPtr->getAttackType() == AttackType::Melee,
          "HardEnemy virtual dispatch works");
      
    std::cout << "\n--- Immediate First Attack Tests ---\n";

    EasyEnemy firstAttackEnemy(
        0, glm::vec2(900.0f, 100.0f), 2.0f
    );

    check(firstAttackEnemy.canAttack(),
          "Enemy can attack immediately after spawning");

    firstAttackEnemy.resetAttackTimer();

    check(!firstAttackEnemy.canAttack(),
          "Enemy cannot attack immediately after reset");

    firstAttackEnemy.update(1.0f);

    check(!firstAttackEnemy.canAttack(),
          "Enemy must wait during cooldown");

    firstAttackEnemy.update(1.0f);

    check(firstAttackEnemy.canAttack(),
          "Enemy can attack again after cooldown");

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
