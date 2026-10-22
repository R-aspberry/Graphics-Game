#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "ui/MainMenu.h"

#include <iostream>

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
    if (!window){
        std::cerr << "Failed to create window \n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD\n";
    return -1;
}
    MainMenu mainMenu;
    mainMenu.init();
    glfwSwapInterval(1);

    float vertices[] = {
        -0.5f, -0.4f,
         0.5f, -0.4f,
         0.5f,  0.4f,

        -0.5f, -0.4f,
        0.5f,  0.4f,
        -0.5f,  0.4f
    };  
    
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    while (!glfwWindowShouldClose(window)){
        int framebufferWidth, framebufferHeight;
        glfwGetFramebufferSize(window,&framebufferWidth,&framebufferHeight);
        glViewport(0, 0, framebufferWidth, framebufferHeight);
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        mainMenu.draw();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}