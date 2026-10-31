#include "PlayerCard.h"
#include <glad/gl.h>
#include "stb_image.h"
#include <iostream>

static const char* cardVertexShaderSource = R"(
    #version 330 core

    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec2 aTex;

    out vec2 TexCoord;

    void main()
    {
        gl_Position = vec4(aPos, 0.0, 1.0);
        TexCoord = aTex;
    }
)";

static const char* cardFragmentShaderSource = R"(
    #version 330 core
    uniform bool hovered;
    in vec2 TexCoord;
    out vec4 FragColor;

    uniform sampler2D cardTexture;

    void main(){
    vec4 color = texture(cardTexture, TexCoord);
    if (hovered)
    {
        color.rgb = min(color.rgb * 1.25, vec3(1.0));
    }

    FragColor = color;
}
)";
void PlayerCard::init()
{
    float cardVertices[] = {
   

    // MELEE 
    -0.90f, 0.55f,   0.0f, 0.0f,
    -0.68f, 0.55f,   1.0f, 0.0f,
    -0.68f, 0.95f,   1.0f, 1.0f,

    -0.90f, 0.55f,   0.0f, 0.0f,
    -0.68f, 0.95f,   1.0f, 1.0f,
    -0.90f, 0.95f,   0.0f, 1.0f,

    // RANGED 
    -0.62f, 0.55f,   0.0f, 0.0f,
    -0.40f, 0.55f,   1.0f, 0.0f,
    -0.40f, 0.95f,   1.0f, 1.0f,

    -0.62f, 0.55f,   0.0f, 0.0f,
    -0.40f, 0.95f,   1.0f, 1.0f,
    -0.62f, 0.95f,   0.0f, 1.0f,

    // DEFENSE 
    -0.34f, 0.55f,   0.0f, 0.0f,
    -0.12f, 0.55f,   1.0f, 0.0f,
    -0.12f, 0.95f,   1.0f, 1.0f,

    -0.34f, 0.55f,   0.0f, 0.0f,
    -0.12f, 0.95f,   1.0f, 1.0f,
    -0.34f, 0.95f,   0.0f, 1.0f
};

glGenVertexArrays(1, &cardVAO);
glGenBuffers(1, &cardVBO);

glBindVertexArray(cardVAO);
glBindBuffer(GL_ARRAY_BUFFER, cardVBO);

glBufferData(GL_ARRAY_BUFFER,sizeof(cardVertices),cardVertices,GL_STATIC_DRAW);

glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,4 * sizeof(float),(void*)0);
glEnableVertexAttribArray(0);


glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,4 * sizeof(float),(void*)(2 * sizeof(float)));
glEnableVertexAttribArray(1);

glBindBuffer(GL_ARRAY_BUFFER, 0);
glBindVertexArray(0);

unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
glShaderSource(vertexShader, 1, &cardVertexShaderSource, nullptr);
glCompileShader(vertexShader);

unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
glShaderSource(fragmentShader, 1, &cardFragmentShaderSource, nullptr);
glCompileShader(fragmentShader);

shaderProgram = glCreateProgram();
glAttachShader(shaderProgram, vertexShader);
glAttachShader(shaderProgram, fragmentShader);
glLinkProgram(shaderProgram);

glDeleteShader(vertexShader);
glDeleteShader(fragmentShader);

glUseProgram(shaderProgram);

int textureLocation =glGetUniformLocation(shaderProgram, "cardTexture");

glUniform1i(textureLocation, 0);


stbi_set_flip_vertically_on_load(true);

int meleeWidth, meleeHeight, meleeChannels;

unsigned char* meleeImage = stbi_load("assets/meleeCrad.png",&meleeWidth, &meleeHeight, &meleeChannels,4);


glGenTextures(1, &meleeTexture);
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, meleeTexture);

glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,meleeWidth, meleeHeight, 0,GL_RGBA, GL_UNSIGNED_BYTE, meleeImage);


stbi_image_free(meleeImage);
glBindTexture(GL_TEXTURE_2D, 0);


int rangedWidth, rangedHeight, rangedChannels;

unsigned char* rangedImage = stbi_load("assets/rangedCard.png",&rangedWidth, &rangedHeight, &rangedChannels, 4);



glGenTextures(1, &rangedTexture);
glBindTexture(GL_TEXTURE_2D, rangedTexture);

glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rangedWidth, rangedHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, rangedImage);

stbi_image_free(rangedImage);
glBindTexture(GL_TEXTURE_2D, 0);


int defenseWidth, defenseHeight, defenseChannels;

unsigned char* defenseImage = stbi_load("assets/defenceCard.png",&defenseWidth, &defenseHeight, &defenseChannels, 4);



glGenTextures(1, &defenseTexture);
glBindTexture(GL_TEXTURE_2D, defenseTexture);

glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,defenseWidth, defenseHeight, 0,GL_RGBA, GL_UNSIGNED_BYTE, defenseImage);

stbi_image_free(defenseImage);
glBindTexture(GL_TEXTURE_2D, 0);


}


void PlayerCard:: draw(int hoveredCard)
{
    glUseProgram(shaderProgram);
    glActiveTexture(GL_TEXTURE0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(cardVAO);

    int hoveredLocation =glGetUniformLocation(shaderProgram, "hovered");
    glUniform1i(hoveredLocation, hoveredCard == 0);
    glBindTexture(GL_TEXTURE_2D, defenseTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glUniform1i(hoveredLocation, hoveredCard == 1);
    glBindTexture(GL_TEXTURE_2D, meleeTexture);
    glDrawArrays(GL_TRIANGLES, 6, 6);

    glUniform1i(hoveredLocation, hoveredCard == 2);
    glBindTexture(GL_TEXTURE_2D, rangedTexture);
    glDrawArrays(GL_TRIANGLES, 12, 6);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDisable(GL_BLEND);
}

