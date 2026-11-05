#include "glad/gl.h"
#include "SoundSettings.h"
#include "Shader.h"
#include "stb_image.h"
#include <iostream>




void SoundSettings::init()
{
    
    float vertices[] = {
       
        -0.70f, -0.10f, 0.0f, 0.0f,
        -0.30f, -0.10f, 1.0f, 0.0f,
        -0.30f,  0.10f, 1.0f, 1.0f,
        -0.70f, -0.10f, 0.0f, 0.0f,
        -0.30f,  0.10f, 1.0f, 1.0f,
        -0.70f,  0.10f, 0.0f, 1.0f,

        
        -0.20f, -0.10f, 0.0f, 0.0f,
         0.20f, -0.10f, 1.0f, 0.0f,
         0.20f,  0.10f, 1.0f, 1.0f,
        -0.20f, -0.10f, 0.0f, 0.0f,
         0.20f,  0.10f, 1.0f, 1.0f,
        -0.20f,  0.10f, 0.0f, 1.0f,

      
         0.30f, -0.10f, 0.0f, 0.0f,
         0.70f, -0.10f, 1.0f, 0.0f,
         0.70f,  0.10f, 1.0f, 1.0f,
         0.30f, -0.10f, 0.0f, 0.0f,
         0.70f,  0.10f, 1.0f, 1.0f,
         0.30f,  0.10f, 0.0f, 1.0f,

       
        -0.25f, -0.60f, 0.0f, 0.0f,
         0.25f, -0.60f, 1.0f, 0.0f,
         0.25f, -0.40f, 1.0f, 1.0f,
        -0.25f, -0.60f, 0.0f, 0.0f,
         0.25f, -0.40f, 1.0f, 1.0f,
        -0.25f, -0.40f, 0.0f, 1.0f
    };

   
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData( GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW );

    
    glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0 );
    glEnableVertexAttribArray(0);

    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,4 * sizeof(float),(void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    
    const char* vertexSource = R"(
        #version 330 core

        layout(location = 0) in vec2 aPos;
        layout(location = 1) in vec2 aTex;

        out vec2 TexCoord;

        void main()
        {
            gl_Position = vec4(aPos, 0.0, 1.0);
            TexCoord = aTex;
        }
    )";

    
    const char* fragmentSource = R"(
        #version 330 core

        in vec2 TexCoord;
        out vec4 FragColor;

        uniform sampler2D tex0;

        void main()
        {
            FragColor = texture(tex0, TexCoord);
        }
    )";

    
    auto compileShader = [](GLenum type, const char* source) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        GLint success = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        return shader;
    };

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource );

    GLuint fragmentShader = compileShader( GL_FRAGMENT_SHADER, fragmentSource);

    if (vertexShader == 0 || fragmentShader == 0) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return;
    }

    
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint linked = 0;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linked);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);



    
    auto loadButtonTexture = [](const char* path) {
        int width, height, channels;

        stbi_set_flip_vertically_on_load(true);

        unsigned char* data = stbi_load(  path, &width, &height, &channels, 4 );

 

        GLuint texture = 0;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(  GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
        glBindTexture(GL_TEXTURE_2D, 0);

        return texture;
    };

   
    volumeDownTexture = loadButtonTexture("assets/sound/down.png" );

    muteTexture = loadButtonTexture( "assets/sound/mute.png");

    volumeUpTexture = loadButtonTexture(  "assets/sound/up.png" );

    backTexture = loadButtonTexture("assets/sound/back.png" );
}



void SoundSettings::draw()
{
    if (shaderProgram == 0 || VAO == 0)
        return;

    glUseProgram(shaderProgram);
    glActiveTexture(GL_TEXTURE0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUniform1i(glGetUniformLocation(shaderProgram, "tex0"), 0);

    glBindVertexArray(VAO);

    
    glBindTexture(GL_TEXTURE_2D, volumeDownTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);


    glBindTexture(GL_TEXTURE_2D, muteTexture);
    glDrawArrays(GL_TRIANGLES, 6, 6);


    glBindTexture(GL_TEXTURE_2D, volumeUpTexture);
    glDrawArrays(GL_TRIANGLES, 12, 6);


    glBindTexture(GL_TEXTURE_2D, backTexture);
    glDrawArrays(GL_TRIANGLES, 18, 6);

  
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDisable(GL_BLEND);
}

SoundAction SoundSettings::handleClick(float x, float y) {
    
    if (x >= -0.70f && x <= -0.30f && y >= -0.10f && y <= 0.10f) {
        return SoundAction::VolumeDown;
    }
    else if (x >= -0.20f && x <= 0.20f && y >= -0.10f && y <= 0.10f) {
        return SoundAction::Mute;
    }
    else if (x >= 0.30f && x <= 0.70f && y >= -0.10f && y <= 0.10f) {
        return SoundAction::VolumeUp;
    }
    else if (x >= -0.25f && x <= 0.25f && y >= -0.60f && y <= -0.40f) {
        return SoundAction::Back;
    }

    return SoundAction::None;
}


void SoundSettings::cleanup()
{
    glDeleteTextures(1, &volumeDownTexture);
    glDeleteTextures(1, &muteTexture);
    glDeleteTextures(1, &volumeUpTexture);
    glDeleteTextures(1, &backTexture);

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(shaderProgram);

    volumeDownTexture = 0;
    muteTexture = 0;
    volumeUpTexture = 0;
    backTexture = 0;

    VBO = 0;
    VAO = 0;
    shaderProgram = 0;
}
