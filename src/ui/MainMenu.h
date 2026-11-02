#pragma once

class MainMenu {
public:
    void init();
    void escTab();
    void endMenu(bool won);
    void draw();
    void cleanup();
  
private:
    unsigned int VAO = 0, VBO = 0;
    unsigned int shaderProgram = 0;
    unsigned int texture = 0;
    unsigned int buttonVAO = 0;
    unsigned int buttonVBO = 0;
    unsigned int buttonTexture = 0; // strat button
    unsigned int exitTexture = 0;

    unsigned int continueButton = 0;
    unsigned int restartButton = 0;    
    unsigned int mainMenuButton = 0;

    unsigned int escVAO = 0;
    unsigned int escVBO = 0;

    unsigned int winTexture = 0;
    unsigned int loseTexture = 0;
    
};
