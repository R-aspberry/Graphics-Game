
#pragma once

#include <glad/gl.h>
#include "Shader.h"

class Renderer
{
public:
    Shader& shader;
    GLuint VAO =0 ;
    GLuint VBO = 0;
    // GLuint EBO;

    float width;
    float height;

    Renderer(Shader& shader, float width, float height);
    ~ Renderer();
    void drawSprite(unsigned int texture, float x,float y, float width, float height);
    void updateWindow(float width, float height);
    void initRender();
};
