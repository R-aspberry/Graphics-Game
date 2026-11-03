
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

    GLuint circleVAO = 0;
    GLuint circleVBO = 0;

    float width;
    float height;

    Renderer(Shader& shader, float width, float height);
    ~ Renderer();
    void drawSprite(unsigned int texture, float x,float y, float width, float height, float damageTint);
    void drawCircle(float x, float y,float width, float height,float r, float g, float b);
    void updateWindow(float width, float height);
    void initRender();

};
