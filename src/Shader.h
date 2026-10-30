#pragma once

#include <glad/gl.h>


class Shader
{
public:
    Shader(const char* vertex,const char* fragment);
    void use() const;
    int ID;
};
