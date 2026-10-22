#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <dwmapi.h>
#include <iostream>

#include "../external/stb/stb_image.h"
#include "world/Grid.h"
#include "ui/MainMenu.h"
    
static const char* vertexShaderSource = R"(
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main() {
    gl_Position = vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
)";
static const char* fragmentShaderSource = R"(
#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D backgroundTexture;

void main() {
    FragColor = texture(backgroundTexture, TexCoord);
}
)";
static GLuint compileShader(GLenum kind, const char* source) {
    GLuint shader = glCreateShader(kind);

    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        char log[1024] = {};
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);

        std::cerr << "Shader error: " << log << '\n';

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}
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

void customizeTitleBar(GLFWwindow* window) {

    HWND hwnd = glfwGetWin32Window(window);

    COLORREF background = RGB(229, 231, 235);
    COLORREF textColor = RGB(17, 24, 39);

    BOOL darkMode = FALSE;

    DwmSetWindowAttribute(hwnd, 20, &darkMode, sizeof(darkMode));

    DwmSetWindowAttribute(hwnd, 35, &background, sizeof(background));
    DwmSetWindowAttribute(hwnd, 36, &textColor, sizeof(textColor));
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(1000, 700, "Tower Defense Game", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create OpenGL window\n";
        glfwTerminate();
        return 1;
    }
   customizeTitleBar(window);

glfwMakeContextCurrent(window);
glfwSwapInterval(1);

if (!gladLoadGL(glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD\n";
    glfwDestroyWindow(window);
    glfwTerminate();
    return 1;
}

MainMenu mainMenu;
mainMenu.init();
glfwSetMouseButtonCallback(window, mouseButtonCallback);
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
    // X      Y      Z       U     V
    -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,
     1.0f, -1.0f, 0.0f,   1.0f, 0.0f,
     1.0f,  1.0f, 0.0f,   1.0f, 1.0f,

    -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,
     1.0f,  1.0f, 0.0f,   1.0f, 1.0f,
    -1.0f,  1.0f, 0.0f,   0.0f, 1.0f
    };

    GLuint vao = 0, vbo = 0;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    int imageWidth = 0;
int imageHeight = 0;
int channels = 0;

stbi_set_flip_vertically_on_load(true);

unsigned char* pixels = stbi_load(
    "assets/backgrounds/cyberpunk.png",
    &imageWidth,
    &imageHeight,
    &channels,
    4
);

if (!pixels) {
    std::cerr << "Failed to load background: "
              << stbi_failure_reason() << '\n';
    glfwDestroyWindow(window);
    glfwTerminate();
    return 1;
}

std::cout << "Background loaded: "
          << imageWidth << " x " << imageHeight << '\n';

GLuint backgroundTexture = 0;

glGenTextures(1, &backgroundTexture);
glBindTexture(GL_TEXTURE_2D, backgroundTexture);

glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

glTexImage2D(
    GL_TEXTURE_2D,
    0,
    GL_RGBA,
    imageWidth,
    imageHeight,
    0,
    GL_RGBA,
    GL_UNSIGNED_BYTE,
    pixels
);

glGenerateMipmap(GL_TEXTURE_2D);

stbi_image_free(pixels);

glUseProgram(program);
glUniform1i(glGetUniformLocation(program, "backgroundTexture"), 0);
    Grid grid;
    for (const Lane& lane : grid.getLanes()) {
    std::cout << "Lane " << lane.id
              << " Top: " << lane.top
              << " Bottom: " << lane.bottom
              << '\n';
}
    int previousHoveredLane = -1;
    int previousLeftMouseState = GLFW_RELEASE;
    while (!glfwWindowShouldClose(window)) {
        int framebufferWidth = 0, framebufferHeight = 0;
        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
        glViewport(0, 0, framebufferWidth, framebufferHeight);
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
       glUseProgram(program);
         glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, backgroundTexture);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glfwSwapBuffers(window);
        glfwPollEvents();
        double mouseX, mouseY;

glfwGetCursorPos(window, &mouseX, &mouseY);

int windowWidth, windowHeight;
glfwGetWindowSize(window, &windowWidth, &windowHeight);

if (windowWidth > 0 && windowHeight > 0) {

    float normalizedX = static_cast<float>(mouseX / windowWidth);
    float normalizedY = static_cast<float>(mouseY / windowHeight);

    int hoveredLane = grid.getLaneAtPosition(normalizedX, normalizedY);

    int leftMouseState = glfwGetMouseButton(
    window,
    GLFW_MOUSE_BUTTON_LEFT
);

if (leftMouseState == GLFW_PRESS &&
    previousLeftMouseState == GLFW_RELEASE &&
    hoveredLane != -1) {

    grid.toggleLaneSelection(hoveredLane);

    std::cout << "Lane " << hoveredLane
              << " selection toggled!" << std::endl;
}

previousLeftMouseState = leftMouseState;

    if (hoveredLane != previousHoveredLane) {

        if (hoveredLane != -1) {
            std::cout << "Hovering over Lane "
                      << hoveredLane << std::endl;
        }
        else {
            std::cout << "Mouse outside lanes" << std::endl;
        }

        previousHoveredLane = hoveredLane;
    }
}
    }
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(program);
    glDeleteTextures(1, &backgroundTexture);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
