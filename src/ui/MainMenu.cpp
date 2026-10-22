#include "MainMenu.h"
#include <glad/glad.h>
#include <iostream>
#ifdef __APPLE__
#include <cstdlib>
#endif
#ifdef __WIN32__
#include <windows.h>
#include <mmsystem.h>
#endif
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec2 aTex;
    out vec2 TexCoord;
    void main() {
        gl_Position = vec4(aPos, 0.0, 1.0);
        TexCoord = aTex;
    }

    layout (location = 3) in vec2 atex ;
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    in vec2 TexCoord;
    uniform sampler2D tex0;
    void main() {
        FragColor = texture(tex0, TexCoord);
    }
)";
   
void MainMenu::init() {

    float startvertices[] = {
    // x      y       u     v
    -1.0f, -1.0f,   0.0f, 0.0f,
     1.0f, -1.0f,   1.0f, 0.0f,
     1.0f,  1.0f,   1.0f, 1.0f,

    -1.0f, -1.0f,   0.0f, 0.0f,
     1.0f,  1.0f,   1.0f, 1.0f,
    -1.0f,  1.0f,   0.0f, 1.0f
};

float buttonVertices[] = {
    // START — left button
    // x      y       u     v
    -0.46f,  0.00f,   0.0f, 0.0f,
    -0.10f,  0.00f,   1.0f, 0.0f,
    -0.10f,  0.20f,   1.0f, 1.0f,

    -0.46f,  0.00f,   0.0f, 0.0f,
    -0.10f,  0.20f,   1.0f, 1.0f,
    -0.46f,  0.20f,   0.0f, 1.0f,

    // EXIT — right button
     0.10f,  0.00f,   0.0f, 0.0f,
     0.46f,  0.00f,   1.0f, 0.0f,
     0.46f,  0.20f,   1.0f, 1.0f,

     0.10f,  0.00f,   0.0f, 0.0f,
     0.46f,  0.20f,   1.0f, 1.0f,
     0.10f,  0.20f,   0.0f, 1.0f
};
glGenVertexArrays(1, &buttonVAO);
glGenBuffers(1, &buttonVBO);
glBindVertexArray(buttonVAO);
glBindBuffer(GL_ARRAY_BUFFER, buttonVBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(buttonVertices), buttonVertices, GL_STATIC_DRAW);
glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);
glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
glEnableVertexAttribArray(1);

glGenVertexArrays(1, &VAO);
glGenBuffers(1, &VBO);

glBindVertexArray(VAO);
glBindBuffer(GL_ARRAY_BUFFER, VBO);

glBufferData(GL_ARRAY_BUFFER,sizeof(startvertices),startvertices,GL_STATIC_DRAW);

glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,4 * sizeof(float), (void*)0);

glEnableVertexAttribArray(0);
glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,4 * sizeof(float), (void*)(2 * sizeof(float)));

glEnableVertexAttribArray(1);
glBindBuffer(GL_ARRAY_BUFFER, 0);
glBindVertexArray(0);
GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
glCompileShader(vertexShader);
GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
glCompileShader(fragmentShader);
shaderProgram = glCreateProgram();
glAttachShader(shaderProgram, vertexShader);
glAttachShader(shaderProgram, fragmentShader);
glLinkProgram(shaderProgram);

#ifdef _WIN32
    PlaySound(
        TEXT("assets/backgroundsound.wav"),
        NULL,
        SND_ASYNC | SND_LOOP | SND_FILENAME
    );
#elif defined(__APPLE__)
    std::system(
        "afplay assets/backgroundsound.wav >/dev/null 2>&1 &"
    );
#endif
int width, height, nrChannels;
stbi_set_flip_vertically_on_load(true);
unsigned char* data = stbi_load("assets/background.png", &width, &height, &nrChannels, 0); // 3shan a import el background bas
glGenTextures(1, &texture);
glActiveTexture(GL_TEXTURE0);

glBindTexture(GL_TEXTURE_2D, texture); //b bind el texture bel el background 

glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);// lama akbr el image el sora hya hya msh hygenerate pixels 3shan tekbr
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
glGenerateMipmap(GL_TEXTURE_2D);
stbi_image_free(data);
glBindTexture(GL_TEXTURE_2D, 0); 
glUseProgram(shaderProgram);

GLint tex0uni = glGetUniformLocation(shaderProgram, "tex0");
glUniform1i(tex0uni, 0);

// button image b2a
int w, h, channels;

// START IMAGE
unsigned char* button = stbi_load(
    "assets/startbutton.png", &w, &h, &channels, 4
);

if (!button)
{
    std::cerr << "Start image failed: "
              << stbi_failure_reason() << '\n';
    return;
}

glGenTextures(1, &buttonTexture);
glBindTexture(GL_TEXTURE_2D, buttonTexture);

glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
             w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, button);

stbi_image_free(button);


// EXIT IMAGE
unsigned char* exitImage = stbi_load(
    "assets/exitbutton.png", &w, &h, &channels, 4
);

if (!exitImage)
{
    std::cerr << "Exit image failed: "
              << stbi_failure_reason() << '\n';
    return;
}

glGenTextures(1, &exitTexture);
glBindTexture(GL_TEXTURE_2D, exitTexture);

glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
             w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, exitImage);

stbi_image_free(exitImage);

glBindTexture(GL_TEXTURE_2D, 0);



}

void MainMenu::draw()
{
    glUseProgram(shaderProgram);
    glActiveTexture(GL_TEXTURE0);

    // Background
    glBindVertexArray(VAO);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Button
glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

// This VAO contains BOTH button rectangles
glBindVertexArray(buttonVAO);

// Select the Start image, then draw the first rectangle
glBindTexture(GL_TEXTURE_2D, buttonTexture);
glDrawArrays(GL_TRIANGLES, 0, 6);

// Select the Exit image, then draw the second rectangle
glBindTexture(GL_TEXTURE_2D, exitTexture);
glDrawArrays(GL_TRIANGLES, 6, 6);

glDisable(GL_BLEND);
}
