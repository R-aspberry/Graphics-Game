#pragma once

#include "glad/gl.h"

class Shader{
    public :
        Shader(const char* vertex, const char* fragment);
        ~ Shader();
        void use();
        GLuint Program =0 ;
    private :
        
        //GLuint compileTheShader(GLenum shaderType , const char* glsl);

    

};