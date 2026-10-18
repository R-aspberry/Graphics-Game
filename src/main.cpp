#define GLAD_GL_IMPLEMENTATION
#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_WIN32

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
void customizeTitleBar(GLFWwindow* window) {

    HWND hwnd = glfwGetWin32Window(window);

    // Light gray title bar
    COLORREF background = RGB(229, 231, 235);

    // Dark text
    COLORREF textColor = RGB(17, 24, 39);

    // Use light appearance
    BOOL darkMode = FALSE;
    DwmSetWindowAttribute(hwnd, 20, &darkMode, sizeof(darkMode));

    // Change title bar colors
    DwmSetWindowAttribute(hwnd, 35, &background, sizeof(background));
    DwmSetWindowAttribute(hwnd, 36, &textColor, sizeof(textColor));

    // Remove minimize and maximize buttons
    LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);

    style &= ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

    SetWindowLongPtr(hwnd, GWL_STYLE, style);

    SetWindowPos(
        hwnd, nullptr,
        0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE |
        SWP_NOZORDER | SWP_FRAMECHANGED
    );
}
int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return 1;
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
