
#include "Shader.h"

#include <iostream>

Shader::Shader(
    const char* vertexSource,
    const char* fragmentSource
)
{
    int success;
    char infoLog[512];

    // Create and compile the vertex shader.
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShader, 1, &vertexSource, nullptr);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(
            vertexShader, 512, nullptr, infoLog
        );

        std::cerr << "Vertex shader compilation failed:\n"
                  << infoLog << '\n';

        glDeleteShader(vertexShader);
        return;
    }

    // Create and compile the fragment shader.
    unsigned int fragmentShader =
        glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(
            fragmentShader, 512, nullptr, infoLog
        );

        std::cerr << "Fragment shader compilation failed:\n"
                  << infoLog << '\n';

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return;
    }

    // Create a program and attach both shaders.
    ID = glCreateProgram();

    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(
            ID, 512, nullptr, infoLog
        );

        std::cerr << "Shader program linking failed:\n"
                  << infoLog << '\n';

        glDeleteProgram(ID);
        ID = 0;
    }

    // Shader objects can be deleted after linking.
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::use() const
{
    if (ID != 0)
    {
        glUseProgram(ID);
    }
}
