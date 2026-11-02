
#include "Shader.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


Shader::Shader (const char* vertex, const char* fragment){
    init(vertex, fragment);
}

bool Shader::init(const char* vertex, const char* fragment) {
    cleanup();
    GLuint vertexID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexID ,1,  &vertex, nullptr);
    glCompileShader(vertexID);
    int success;
    glGetShaderiv(vertexID, GL_COMPILE_STATUS, &success);

    if(!success){
        glDeleteShader(vertexID);
        return false;
    }

    GLuint fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentID ,1,  &fragment, nullptr);
    glCompileShader(fragmentID);
    glGetShaderiv(fragmentID, GL_COMPILE_STATUS, &success);

    if(!success){
        glDeleteShader(fragmentID);
        glDeleteShader(vertexID);
        return false;
    }

    Program = glCreateProgram();

    glAttachShader(Program, vertexID);
    glAttachShader(Program, fragmentID);

    glLinkProgram(Program);

    glGetProgramiv(Program, GL_LINK_STATUS, &success);

    if(!success){
        glDeleteProgram(Program);
        Program = 0;
       
    }

    glDeleteShader(vertexID);
    glDeleteShader(fragmentID);
    return Program != 0;
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
    cleanup();
}

void Shader::cleanup() {
    if (Program != 0 && glfwGetCurrentContext()) glDeleteProgram(Program);
    Program = 0;
}
