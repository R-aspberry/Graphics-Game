
#include "Shader.h"


Shader::Shader (const char* vertex, const char* fragment){
    GLuint vertexID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexID ,1,  &vertex, nullptr);
    glCompileShader(vertexID);
    int success;
    glGetShaderiv(vertexID, GL_COMPILE_STATUS, &success);

    if(!success){
        glDeleteShader(vertexID);
        return ;
    }

    GLuint fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentID ,1,  &fragment, nullptr);
    glCompileShader(fragmentID);
    glGetShaderiv(fragmentID, GL_COMPILE_STATUS, &success);

    if(!success){
        glDeleteShader(fragmentID);
        glDeleteShader(vertexID);
        return ;
    }

    Program = glCreateProgram();

    glAttachShader(Program, vertexID);
    glAttachShader(Program, fragmentID);

    glLinkProgram(Program);

    glGetProgramiv(Program, GL_LINK_STATUS, &success);

    if(!success){
        glDeleteShader(vertexID);
        glDeleteShader(fragmentID);
        glDeleteProgram(Program);
        Program = 0;
       
    }

    glDeleteShader(vertexID);
    glDeleteShader(fragmentID);



}

void Shader::use(){
    if(Program != 0){
        glUseProgram(Program);
    }
}

// ~ Shader(){
//     glDeleteProgram(Program);
// }

Shader::~Shader(){
    if (Program != 0)
        glDeleteProgram(Program);
}