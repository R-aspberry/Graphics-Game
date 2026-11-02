#include "Cutscene.h"
#include <algorithm>
#include <cmath>

void Cutscene::start() { elapsed = 0; playing = true; }
void Cutscene::skip() { playing = false; }
void Cutscene::update(float deltaTime) {
    if (!playing || !std::isfinite(deltaTime) || deltaTime < 0) return;
    elapsed = std::min(12.0f, elapsed + deltaTime);
    if (elapsed >= 12) playing = false;
}
float Cutscene::getPlayerX() const { return .18f + .20f * std::min(elapsed / 3, 1.0f); }
float Cutscene::getEnemyX() const { return .88f - .27f * std::min(elapsed / 4, 1.0f); }
const char* Cutscene::getDialogue() const {
    if (elapsed < 3) return "GUARD: THE CITY CORE MUST STAY SAFE";
    if (elapsed < 6) return "RAIDER: YOUR CORE WILL BE OURS";
    if (elapsed < 9) return "GUARD: HOLD THE LANES AND PROTECT THE CITY";
    return "GUARD: DUCK HIGH SHOTS AND JUMP LOW SHOTS";
}
