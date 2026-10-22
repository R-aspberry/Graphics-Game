
#include "Renderer.h"

Renderer::Renderer(Shader& shader)
    : shader(shader)
{
    // Each vertex contains position (x, y) and texture coordinates (u, v).
    float vertices[] = {
        // Position       // Texture coordinates
        0.0f, 1.0f,        0.0f, 1.0f,
        1.0f, 0.0f,        1.0f, 0.0f,
        0.0f, 0.0f,        0.0f, 0.0f,

        0.0f, 1.0f,        0.0f, 1.0f,
        1.0f, 1.0f,        1.0f, 1.0f,
        1.0f, 0.0f,        1.0f, 0.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );

    // Attribute 0: position.
    glVertexAttribPointer(
        0, 2, GL_FLOAT, GL_FALSE,
        4 * sizeof(float), (void*)0
    );
    glEnableVertexAttribArray(0);

    // Attribute 1: texture coordinates.
    glVertexAttribPointer(
        1, 2, GL_FLOAT, GL_FALSE,
        4 * sizeof(float), (void*)(2 * sizeof(float))
    );
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Renderer::drawSprite(
    unsigned int texture,
    float x,
    float y,
    float width,
    float height
)
{
    if (shader.ID == 0)
    {
        return;
    }

    shader.use();

    GLint rectLocation =
        glGetUniformLocation(shader.ID, "spriteRect");

    glUniform4f(rectLocation, x, y, width, height);

    GLint imageLocation =
        glGetUniformLocation(shader.ID, "image");

    glUniform1i(imageLocation, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glDisable(GL_BLEND);
}
