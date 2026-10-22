#define GLAD_GL_IMPLEMENTATION
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "ui/MainMenu.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <dwmapi.h>
#include <iostream>
    
static const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
void main() { gl_Position = vec4(aPos, 1.0); }
)";
static const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
void main() { FragColor = vec4(1.0, 0.5, 0.2, 1.0); }
)";

enum class GameState {
    MENU,
    CUTSCENE,
    PLAYING
};

GameState gameState = GameState::MENU;

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        if (gameState == GameState::MENU)
        {
            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);

            int width, height;
            glfwGetWindowSize(window, &width, &height);

            if (width <= 0 || height <= 0)
            {
                return; // Avoid division by zero el window mmkn teb2a feha moshkela
            }

            double x = (2.0 * mouseX / width) - 1.0; // Convert mouse x w y to OpenGL coordinates 3shan y align 
            double y = 1.0 - (2.0 * mouseY / height);

          // Start rectangle
if (x >= -0.46 && x <= -0.10 &&
    y >= 0.00 && y <= 0.20)
{
    gameState = GameState::CUTSCENE;
    std::cout << "Start clicked!\n";
}
// Exit rectangle
else if (x >= 0.10 && x <= 0.46 &&
         y >= 0.00 && y <= 0.20)
{
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}
        }
    }
}

int main(){
    if (!glfwInit()){
        std::cerr << "Failed to initialize window \n";
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1000, 700, "Tower Defense Game", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create OpenGL window\n";
        glfwTerminate();
        return 1;
    }
    customizeTitleBar(window);
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD\n";
    return -1;
}
    MainMenu mainMenu;
    mainMenu.init();
    glfwSwapInterval(1);
    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }
    GLint major = 0, minor = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    std::cout << "OpenGL " << major << "." << minor << " initialized\n";

    GLuint vert = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint frag = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    if (!vert || !frag) {
        if (vert) glDeleteShader(vert);
        if (frag) glDeleteShader(frag);
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }
    GLuint program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);
    GLint linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    glDeleteShader(vert);
    glDeleteShader(frag);
    if (!linked) {
        char log[1024] = {};
        glGetProgramInfoLog(program, sizeof(log), nullptr, log);
        std::cerr << "Shader program link error: " << log << '\n';
        glDeleteProgram(program);
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    const float vertices[] = {
        -0.5f, -0.4f, 0.0f,
         0.5f, -0.4f, 0.0f,
         0.5f,  0.4f, 0.0f,
        -0.5f, -0.4f, 0.0f,
         0.5f,  0.4f, 0.0f,
        -0.5f,  0.4f, 0.0f
    };
    GLuint vao = 0, vbo = 0;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window)) {
        int framebufferWidth = 0, framebufferHeight = 0;
        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
        glViewport(0, 0, framebufferWidth, framebufferHeight);
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        mainMenu.draw();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(program);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
