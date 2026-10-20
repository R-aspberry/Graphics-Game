#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "ui/MainMenu.h"

#include <iostream>
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