#pragma once

#include <glad/glad.h>


class Shader
{
public:
    Shader(const char* vertex,const char* fragment);
    void use() const;
    int ID;
};
