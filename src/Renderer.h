
#pragma once

#include <glad/gl.h>
#include "Shader.h"

class Renderer
{
public:
    explicit Renderer(Shader& shader);

    void drawSprite(
        unsigned int texture,
        float x,
        float y,
        float width,
        float height
    );

private:
    Shader& shader;

    unsigned int VAO = 0;
    unsigned int VBO = 0;
};
