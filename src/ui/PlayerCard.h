#pragma once

class PlayerCard
{
public:
    void init();
    void draw(int hoveredCard);

private:
    unsigned int cardVAO = 0;
    unsigned int cardVBO = 0;
    unsigned int shaderProgram = 0;

    unsigned int meleeTexture = 0;
    unsigned int rangedTexture = 0;
    unsigned int defenseTexture = 0;
};