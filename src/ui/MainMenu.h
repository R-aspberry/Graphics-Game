#pragma once

class MainMenu {
public:
    void init();
    void draw();

private:
    unsigned int VAO, VBO;
    unsigned int shaderProgram;
    unsigned int texture = 0;
};