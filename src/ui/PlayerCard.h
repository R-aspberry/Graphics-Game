#pragma once

class PlayerCard
{
public:
    void init();
    // cards are ordered Defense, Melee, Ranged to match the on-screen layout
    void draw(int hoveredCard, bool defenseUnlocked, bool meleeUnlocked,
              bool rangedUnlocked);

private:
    unsigned int cardVAO = 0;
    unsigned int cardVBO = 0;
    unsigned int shaderProgram = 0;

    unsigned int meleeTexture = 0;
    unsigned int rangedTexture = 0;
    unsigned int defenseTexture = 0;
};
