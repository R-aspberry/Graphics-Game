#include "Cutscene.h"
#include "utils/ResourceManager.h"
#include "../systems/SoundManager.h"

#include <iostream>


const char *cutsceneVertexShader = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTex;
out vec2 TexCoord;
uniform vec4 rect;
uniform int flipX;
void main()
{
    gl_Position = vec4(rect.x + aPos.x * rect.z,rect.y + aPos.y * rect.w, 0.0, 1.0);
    TexCoord = aTex;
    if (flipX == 1)
        TexCoord.x = 1.0 - TexCoord.x;
}
)";

const char *cutsceneFragmentShader = R"(
#version 330 core
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D image;
uniform float opacity;
void main()
{
    FragColor = texture(image, TexCoord);
   
}
)";


static GLuint compileShader(GLenum type, const char *source){
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char log[512] = {};
        glGetShaderInfoLog(shader, 512, nullptr, log);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

bool Cutscene::init(){
    if (ready)
        return true;

    GLuint vertex = compileShader(GL_VERTEX_SHADER, cutsceneVertexShader);
    GLuint fragment = compileShader(GL_FRAGMENT_SHADER, cutsceneFragmentShader);

    if (vertex == 0 || fragment == 0){
        if (vertex != 0)
            glDeleteShader(vertex);
        if (fragment != 0)
            glDeleteShader(fragment);
        return false;
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertex);
    glAttachShader(shaderProgram, fragment);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    int success = 0;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    
    if (!success){
        cleanup();
        return false;
    }

  
    float vertices[] = {
       
        0, 0, 0, 0,
        1, 0, 1, 0,
        1, 1, 1, 1,
        0, 0, 0, 0,
        1, 1, 1, 1,
        0, 1, 0, 1};

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    
    background = ResourceManager::loadTexture("assets/cutscene/Neon Energy Core Headquarters.png");
    signal = ResourceManager::loadTexture("assets/cutscene/signal.png");
    enemyText = ResourceManager::loadTexture("assets/cutscene/enemy_line.png");
    playerText = ResourceManager::loadTexture("assets/cutscene/hero_lane2.png");
    title = ResourceManager::loadTexture("assets/cutscene/title.png");
    story = ResourceManager::loadTexture("assets/cutscene/story.png");
    objective = ResourceManager::loadTexture("assets/cutscene/objective.png");
    skipText = ResourceManager::loadTexture("assets/cutscene/skip.png");

   
    playerWalk[0] = ResourceManager::loadTexture("assets/sprites/Melee_M1.png");
    playerWalk[1] = ResourceManager::loadTexture("assets/sprites/Melee_M2.png");
    playerWalk[2] = ResourceManager::loadTexture("assets/sprites/Melee_M3.png");
    playerWalk[3] = ResourceManager::loadTexture("assets/sprites/Melee_M4.png");

    playerAttack[0] = ResourceManager::loadTexture("assets/sprites/Melee_A1.png");
    playerAttack[1] = ResourceManager::loadTexture("assets/sprites/Melee_A2.png");
    playerAttack[2] = ResourceManager::loadTexture("assets/sprites/Melee_A3.png");
    playerAttack[3] = ResourceManager::loadTexture("assets/sprites/Melee_A4.png");

    enemyWalk[0] = ResourceManager::loadTexture("assets/sprites/enemies/EASYENEMY/EASYENEMY_M1.png");
    enemyWalk[1] = ResourceManager::loadTexture("assets/sprites/enemies/EASYENEMY/EASYENEMY_M2.png");
    enemyWalk[2] = ResourceManager::loadTexture("assets/sprites/enemies/EASYENEMY/EASYENEMY_M3.png");
    enemyWalk[3] = ResourceManager::loadTexture("assets/sprites/enemies/EASYENEMY/EASYENEMY_M4.png");

    enemyAttack[0] = ResourceManager::loadTexture("assets/sprites/enemies/EASYENEMY/EASYENEMY_A1.png");
    enemyAttack[1] = ResourceManager::loadTexture("assets/sprites/enemies/EASYENEMY/EASYENEMY_A2.png");
    enemyAttack[2] = ResourceManager::loadTexture("assets/sprites/enemies/EASYENEMY/EASYENEMY_A3.png");
    enemyAttack[3] = ResourceManager::loadTexture("assets/sprites/enemies/EASYENEMY/EASYENEMY_A4.png");

    enemyDeath[0] = ResourceManager::loadTexture("assets/sprites/enemies/EASYENEMY/EASYENEMY_D1.png");
    enemyDeath[1] = ResourceManager::loadTexture("assets/sprites/enemies/EASYENEMY/EASYENEMY_D2.png");
    enemyDeath[2] = ResourceManager::loadTexture("assets/sprites/enemies/EASYENEMY/EASYENEMY_D3.png");
    enemyDeath[3] = ResourceManager::loadTexture("assets/sprites/enemies/EASYENEMY/EASYENEMY_D4.png");

    ready = background != 0 && signal != 0 && title != 0 &&
            playerWalk[0] != 0 && playerAttack[0] != 0 &&
            enemyWalk[0] != 0 && enemyAttack[0] != 0 && enemyDeath[0] != 0;


    return ready;
}

void Cutscene::start(){
    time = 0.0f;
    musicPlaying = SoundManager::instance().playMusic("assets/cutscene/cinematic.wav", false);
}

void Cutscene::reset(){
    time = 0.0f;
}

void Cutscene::update(float deltaTime){
    if (deltaTime > 0.0f)
        time += deltaTime;
}

bool Cutscene::finished() const{
    return time >= 18.0f;
}

void Cutscene::stopAudio(){
    if (musicPlaying)
    {
        SoundManager::instance().stopMusic();
        musicPlaying = false;
    }
}

void Cutscene::drawImage(GLuint texture, float x, float y, float width, float height, float opacity, bool flip){
    if (texture == 0)
        return;

    glUseProgram(shaderProgram);
    glUniform4f(glGetUniformLocation(shaderProgram, "rect"), x, y, width, height);
    glUniform1f(glGetUniformLocation(shaderProgram, "opacity"), opacity);
    glUniform1i(glGetUniformLocation(shaderProgram, "flipX"), flip ? 1 : 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "image"), 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Cutscene::draw(int screenWidth, int screenHeight){
    if (!ready || screenWidth <= 0 || screenHeight <= 0)
        return;

    glViewport(0, 0, screenWidth, screenHeight);
    glClearColor(0.01f, 0.01f, 0.06f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float bgWidth = 2.0f * (1967.0f / 799.0f) * screenHeight / screenWidth;
    if (bgWidth < 2.0f)
        bgWidth = 2.0f;
    drawImage(background, -bgWidth / 2.0f, -1.0f, bgWidth, 2.0f);

    int frame = (int)(time * 7.0f) % 4;

    if (time < 3.0f){
        drawImage(signal, -0.85f, 0.35f, 1.7f, 0.4f);
        drawImage(story, -0.8f, -1.25f, 1.6f, 0.9f);
    }

    if (time >= 3.0f && time < 13.0f){
        float enemyX = 0.95f;
        if (time < 10.0f)
            enemyX = 0.95f - (time - 3.0f) * 0.12f;
        else
            enemyX = 0.11f;

        if (time >= 10.0f)
            drawImage(enemyAttack[frame], enemyX, -0.68f, 0.48f, 0.82f, 1.0f, true);
        else
            drawImage(enemyWalk[frame], enemyX, -0.68f, 0.48f, 0.82f, 1.0f, true);
    }

    if (time >= 4.0f && time < 6.7f)
        drawImage(enemyText, -0.8f, -0.92f, 1.6f, 0.3f);

    if (time >= 6.0f && time < 15.0f)
    {
        float playerX = -1.05f;
        if (time < 10.0f)
            playerX = -1.05f + (time - 6.0f) * 0.17f;
        else
            playerX = -0.37f;

        if (time >= 10.0f && time < 13.0f)
            drawImage(playerAttack[frame], playerX, -0.68f, 0.53f, 0.99f);
        else if (time >= 13.0f)
            drawImage(playerWalk[0], playerX, -0.68f, 0.53f, 0.99f);
        else
            drawImage(playerWalk[frame], playerX, -0.68f, 0.53f, 0.99f);
    }


    if (time >= 7.0f && time < 12.0f)
        drawImage(playerText, -0.8f, -1.25f, 1.6f, 0.9f);

    // if (time >= 11.0f && time < 11.13f)
    // {
    //     glClearColor(0.65f, 0.55f, 0.9f, 1.0f);
    //     glClear(GL_COLOR_BUFFER_BIT);
    // }

    //if (time >= 12.0f && time < 17.0f)

    if (time >= 13.0f && time < 15.0f){
        int deathFrame = (int)((time - 13.0f) * 2.0f);
        if (deathFrame > 3)
            deathFrame = 3;
        drawImage(enemyDeath[deathFrame], 0.11f, -0.68f, 0.48f, 0.82f, 1.0f, true);
    }

    if (time >= 15.0f){
        glClearColor(0.01f, 0.01f, 0.06f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        drawImage(title, -0.85f, -0.05f, 1.7f, 0.58f);
        drawImage(objective, -0.80f, -0.50f, 1.6f, 0.27f);
    }

    drawImage(skipText, 0.50f, -0.96f, 0.55f, 0.13f);
    glDisable(GL_BLEND);
    glUseProgram(0);
}

void Cutscene::cleanup(){
    stopAudio();
    if (VBO != 0)
        glDeleteBuffers(1, &VBO);
    if (VAO != 0)
        glDeleteVertexArrays(1, &VAO);
    if (shaderProgram != 0)
        glDeleteProgram(shaderProgram);
    VBO = 0;
    VAO = 0;
    shaderProgram = 0;
    ready = false;
}
