#include "Renderer.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

Renderer::Renderer(Shader& shader, float width, float height)
    : shader(shader), width(width), height(height) {
    initRender();
}
void Renderer::initRender() {
    float vertices[] = {
        0, 1, 0, 1,  1, 0, 1, 0,  0, 0, 0, 0,
        0, 1, 0, 1,  1, 0, 1, 0,  1, 1, 1, 1
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}
void Renderer::drawSprite(unsigned int texture, float x, float y, float w, float h,
                          float red, float green, float blue, float alpha) {
    if (shader.Program == 0 || texture == 0 || w <= 0 || h <= 0) return;
    shader.use();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUniform4f(glGetUniformLocation(shader.Program, "spriteRect"), x, y, w, h);
    glUniform4f(glGetUniformLocation(shader.Program, "spriteTint"), red, green, blue, alpha);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(shader.Program, "image"), 0);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
void Renderer::updateWindow(float newWidth, float newHeight) {
    if (newWidth > 0) width = newWidth;
    if (newHeight > 0) height = newHeight;
}
Renderer::~Renderer() {
    if (glfwGetCurrentContext()) {
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);
    }
}
