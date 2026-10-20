#include "MainMenu.h"
#include <glad/glad.h>
#include <iostream>
#include <windows.h>
#include <mmsystem.h>
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

sndPlaySound("assets/backgroundsound.wav", SND_ASYNC | SND_LOOP | SND_NOSTOP);

int width, height, nrChannels;
stbi_set_flip_vertically_on_load(true);
unsigned char* data = stbi_load("assets/background.jpg", &width, &height, &nrChannels, 0); // 3shan a import el background bas
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

}

void MainMenu::draw() {
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glBindTexture(GL_TEXTURE_2D, texture); //b bind el texture bel el background 
    glDrawArrays(GL_TRIANGLES, 0, 6);
}