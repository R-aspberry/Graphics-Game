#pragma once

class MainMenu {
public:
    void init();
    void draw();

private:
    unsigned int VAO, VBO;
    unsigned int shaderProgram;
    unsigned int texture = 0;
    unsigned int buttonVAO = 0;
    unsigned int buttonVBO = 0;
    unsigned int buttonTexture = 0; // strat button
    unsigned int exitTexture = 0;

};