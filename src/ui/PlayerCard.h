#pragma once
class Economy;

class PlayerCard
{
public:
    void init();
    void draw(int hoveredCard, const Economy* economy = nullptr);
    void cleanup();

private:
    unsigned int cardVAO = 0;
    unsigned int cardVBO = 0;
    unsigned int shaderProgram = 0;

    unsigned int meleeTexture = 0;
    unsigned int rangedTexture = 0;
    unsigned int defenseTexture = 0;
};
