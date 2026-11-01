
#include "Renderer.h"

Renderer::Renderer(Shader& shader, float width, float height): shader(shader), width(width), height(height){
    shader.use();
    initRender();
}

void Renderer::initRender(){
    float vertices[] = { 0.0f , 1.0f ,  0.0f , 1.0f ,
          1.0f , 0.0f , 1.0f , 0.0f ,
         0.0f , 0.0f ,  0.0f , 0.0f ,
        
        0.0f , 1.0f ,  0.0f , 1.0f ,
          1.0f , 0.0f , 1.0f , 0.0f ,
         1.0f , 1.0f ,  1.0f , 1.0f };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices), vertices,GL_STATIC_DRAW);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(vertices), vertices,GL_STATIC_DRAW);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);
    
}

void Renderer::drawSprite(unsigned int texture,float x,float y, float w,float h){
    if (shader.Program == 0)
    {
        return;
    }

    shader.use();

    float newX = ((2 * x )/ w) - 1;
    float newY = 1- ((2 * y)/ h);
    float newW = ((2 * w )/ Renderer::width);
    float newH = ((2 * h )/ Renderer::height);

    GLint rectLocation = glGetUniformLocation(shader.Program, "spriteRect");
    glUniform4f(rectLocation, x , y, w , h);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    GLint imageLocation = glGetUniformLocation(shader.Program, "image");

    glUniform1i(imageLocation, 0);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // GLint rectLocation =
    //     glGetUniformLocation(shader.Program, "spriteRect");

    // glUniform4f(rectLocation, x, y, width, height);

    // GLint imageLocation =
    //     glGetUniformLocation(shader.ID, "image");

    // glUniform1i(imageLocation, 0);

    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, texture);

    // glBindVertexArray(VAO);
    // glDrawArrays(GL_TRIANGLES, 0, 6);

    // glBindVertexArray(0);
    // glDisable(GL_BLEND);
}


// void Renderer::updateWindow (float width, float height){
//     Renderer::width = width;
//     Renderer::height = height;
// }

Renderer::~Renderer(){
    glDeleteBuffers(1, &VBO);
    // glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}