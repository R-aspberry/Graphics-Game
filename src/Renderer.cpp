
#include "Renderer.h"

#include <cmath>
#include <vector>

Renderer::Renderer(Shader& shader, float width, float height): shader(shader), width(width), height(height){
    shader.use();
    initRender();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

void Renderer::drawSprite(unsigned int texture,float x,float y, float w,float h, float damageTint){
    if (shader.Program == 0)
    {
        return;
    }

    shader.use();

    GLint damageTintLocation = glGetUniformLocation(shader.Program, "damageTint");
    glUniform1f(damageTintLocation, damageTint); // the uniform methods take location and feature to applt

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

void Renderer::drawCircle(float x, float y,float width, float height,float r, float g, float b){
    if(shader.Program == 0)
        return;
    const int segments = 36;
    const float PI = 3.14159265f;
    if(circleVAO == 0){
        std::vector<float> vertices;

        // Center of circle
        vertices.push_back(0.5f);
        vertices.push_back(0.5f);

        // Points around the circle
        for (int i = 0; i <= segments; i++)
        {
            float angle = 2.0f * PI * i / segments;

            float px = 0.5f + 0.5f * std::cos(angle);
            float py = 0.5f + 0.5f * std::sin(angle);

            vertices.push_back(px);
            vertices.push_back(py);
        }

        glGenVertexArrays(1, &circleVAO);
        glGenBuffers(1, &circleVBO);

        glBindVertexArray(circleVAO);
        glBindBuffer(GL_ARRAY_BUFFER, circleVBO);

        glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW );

        glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0 );

        glEnableVertexAttribArray(0);
    }
    shader.use();

    // Position and size
    glUniform4f( glGetUniformLocation(shader.Program, "spriteRect"), x, y, width, height);

    // Enable solid-color rendering
    glUniform1i( glGetUniformLocation(shader.Program, "solidColor"), 1 );

    // Circle color
    glUniform3f( glGetUniformLocation(shader.Program, "circleColor"), r, g, b );

    glBindVertexArray(circleVAO);

    glDrawArrays(GL_TRIANGLE_FAN, 0, segments + 2);

    glBindVertexArray(0);

    // Restore normal sprite rendering
    glUniform1i( glGetUniformLocation(shader.Program, "solidColor"), 0 );
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