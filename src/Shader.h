#pragma once

#include "glad/gl.h"

class Shader{
    public :
        Shader() = default;
        Shader(const char* vertex, const char* fragment);
        bool init(const char* vertex, const char* fragment);
        void cleanup();
        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;
        ~ Shader();
        void use();
        GLuint Program =0 ;
    private :
        
        //GLuint compileTheShader(GLenum shaderType , const char* glsl);

    

};
