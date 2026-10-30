
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
#include "Game.h"
#include "systems/Economy.h"
// =====================================
// SHADERS
// =====================================

static const char* vertexShaderSource = R"(
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

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
uniform vec4 laneBounds[4];
uniform vec4 selectedFlags;
uniform int hoveredLane;

void main() {
    vec4 color = texture(backgroundTexture, TexCoord);

    float x = TexCoord.x;
    float y = 1.0 - TexCoord.y;

    for (int i = 0; i < 4; i++) {
        vec4 bounds = laneBounds[i];

        if (x >= bounds.x &&
            x <= bounds.y &&
            y >= bounds.z &&
            y <= bounds.w) {

            if (selectedFlags[i] > 0.5) {
                color.rgb = mix(
                    color.rgb,
                    vec3(0.6, 0.2, 1.0),
                    0.4
                );
            }
            else if (hoveredLane == i) {
                color.rgb = mix(
                    color.rgb,
                    vec3(0.0, 0.9, 1.0),
                    0.3
                );
            }
        }
    }

    FragColor = color;
}
)";

// =====================================
// COMPILE SHADER
// =====================================

static GLuint compileShader(GLenum kind, const char* source) {
    GLuint shader = glCreateShader(kind);

    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        char log[1024] = {};

        glGetShaderInfoLog(
            shader,
            sizeof(log),
            nullptr,
            log
        );

        std::cerr << "Shader error: " << log << '\n';

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

// =====================================
// SCREEN STATE
// =====================================

enum class AppScreen {
    MENU,
    CUTSCENE,
    PLAYING
};

static AppScreen currentScreen = AppScreen::MENU;

// Prevent the Start button click from also
// selecting a lane underneath it.

static bool suppressGameplayClick = false;

// =====================================
// MOUSE CALLBACK
// =====================================

void mouseButtonCallback(
    GLFWwindow* window,
    int button,
    int action,
    int mods
) {
    (void)mods;

    if (button != GLFW_MOUSE_BUTTON_LEFT ||
        action != GLFW_PRESS) {
        return;
    }

    if (currentScreen != AppScreen::MENU) {
        return;
    }

    double mouseX = 0.0;
    double mouseY = 0.0;

    glfwGetCursorPos(window, &mouseX, &mouseY);

    int width = 0;
    int height = 0;

    glfwGetWindowSize(window, &width, &height);

    if (width <= 0 || height <= 0) {
        return;
    }

    double x = (2.0 * mouseX / width) - 1.0;
    double y = 1.0 - (2.0 * mouseY / height);

    // START BUTTON

    if (x >= -0.46 && x <= -0.10 &&
        y >= 0.00 && y <= 0.20) {

        // Go directly to gameplay for now.
        // CUTSCENE can be connected later.

        currentScreen = AppScreen::PLAYING;
        suppressGameplayClick = true;

        std::cout << "Start clicked!\n";
    }

    // EXIT BUTTON

    else if (x >= 0.10 && x <= 0.46 &&
             y >= 0.00 && y <= 0.20) {

        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

// =====================================
// WINDOWS TITLE BAR
// =====================================

void customizeTitleBar(GLFWwindow* window) {
    HWND hwnd = glfwGetWin32Window(window);

    COLORREF background = RGB(229, 231, 235);
    COLORREF textColor = RGB(17, 24, 39);

    BOOL darkMode = FALSE;

    DwmSetWindowAttribute(
        hwnd,
        20,
        &darkMode,
        sizeof(darkMode)
    );

    DwmSetWindowAttribute(
        hwnd,
        35,
        &background,
        sizeof(background)
    );

    DwmSetWindowAttribute(
        hwnd,
        36,
        &textColor,
        sizeof(textColor)
    );
}

// =====================================
// MAIN
// =====================================

int main() {

    // INITIALIZE GLFW

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE
    );

    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    // CREATE WINDOW

    GLFWwindow* window = glfwCreateWindow(
        1000,
        700,
        "Tower Defense Game",
        nullptr,
        nullptr
    );

    if (!window) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return 1;
    }

    customizeTitleBar(window);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // INITIALIZE GLAD

    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";

        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    GLint major = 0;
    GLint minor = 0;

    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);

    std::cout << "OpenGL "
              << major << "."
              << minor << " initialized\n";

    // CREATE MENU AND GAME

    MainMenu mainMenu;
    mainMenu.init();

    Game game;
    bool gameInitialized = false;

    glfwSetMouseButtonCallback(
        window,
        mouseButtonCallback
    );

    // =====================================
    // CREATE SHADER PROGRAM
    // =====================================

    GLuint vert = compileShader(
        GL_VERTEX_SHADER,
        vertexShaderSource
    );

    GLuint frag = compileShader(
        GL_FRAGMENT_SHADER,
        fragmentShaderSource
    );

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

    glGetProgramiv(
        program,
        GL_LINK_STATUS,
        &linked
    );

    glDeleteShader(vert);
    glDeleteShader(frag);

    if (!linked) {
        char log[1024] = {};

        glGetProgramInfoLog(
            program,
            sizeof(log),
            nullptr,
            log
        );

        std::cerr << "Shader program link error: "
                  << log << '\n';

        glDeleteProgram(program);
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    // =====================================
    // BACKGROUND VERTICES
    // =====================================

    const float vertices[] = {
        // X       Y      Z      U      V

        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,

        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f
    };

    GLuint vao = 0;
    GLuint vbo = 0;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        5 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        5 * sizeof(float),
        (void*)(3 * sizeof(float))
    );

    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // =====================================
    // LOAD CYBERPUNK BACKGROUND
    // =====================================

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
                  << stbi_failure_reason()
                  << '\n';

        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
        glDeleteProgram(program);

        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    std::cout << "Background loaded: "
              << imageWidth
              << " x "
              << imageHeight
              << '\n';

    GLuint backgroundTexture = 0;

    glGenTextures(1, &backgroundTexture);
    glBindTexture(GL_TEXTURE_2D, backgroundTexture);

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_S,
        GL_CLAMP_TO_EDGE
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_T,
        GL_CLAMP_TO_EDGE
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_LINEAR_MIPMAP_LINEAR
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER,
        GL_LINEAR
    );

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

    // =====================================
    // GRID
    // =====================================

    Grid grid;
    int playerLaneId = 0; // Initial lane; changes when the player is assigned
    Economy economy;

    std::cout << "Starting coins: "
              << economy.getCoins() << std::endl;

    std::cout << "Can afford Melee? "
              << (economy.canAfford(10) ? "Yes" : "No")
              << std::endl;

    for (const Lane& lane : grid.getLanes()) {
        std::cout << "Lane "
                  << lane.id
                  << " Top: "
                  << lane.top
                  << " Bottom: "
                  << lane.bottom
                  << '\n';
    }

    int previousHoveredLane = -1;
    int previousLeftMouseState = GLFW_RELEASE;
    int previousBuyKeyState = GLFW_RELEASE;

    double lastFrameTime = glfwGetTime();

    // =====================================
    // ONE MAIN GAME LOOP
    // =====================================

    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents();

        double currentTime = glfwGetTime();

        float deltaTime = static_cast<float>(
            currentTime - lastFrameTime
        );

        lastFrameTime = currentTime;

        int framebufferWidth = 0;
        int framebufferHeight = 0;

        glfwGetFramebufferSize(
            window,
            &framebufferWidth,
            &framebufferHeight
        );

        if (framebufferWidth <= 0 ||
            framebufferHeight <= 0) {
            continue;
        }

        glViewport(
            0,
            0,
            framebufferWidth,
            framebufferHeight
        );

        glClearColor(
            0.1f,
            0.1f,
            0.15f,
            1.0f
        );

        glClear(GL_COLOR_BUFFER_BIT);

        // =====================================
        // MAIN MENU
        // =====================================

        if (currentScreen == AppScreen::MENU) {

            mainMenu.draw();
        }

        // =====================================
        // GAMEPLAY
        // =====================================

        else if (currentScreen == AppScreen::PLAYING) {

            // Initialize game ONCE

            if (!gameInitialized) {
                game.init();
                gameInitialized = true;
            }

            
    // =====================================
    // TEST MELEE PURCHASE
    // =====================================

    int buyKeyState = glfwGetKey(window, GLFW_KEY_M);

    if (buyKeyState == GLFW_PRESS &&
        previousBuyKeyState == GLFW_RELEASE) {

        const int meleeCost = 10;

        if (economy.spendCoins(meleeCost)) {

            std::cout << "Purchase test successful!\n";

            std::cout << "Remaining coins: "
                      << economy.getCoins()
                      << std::endl;
        }
        else {

            std::cout << "Not enough coins!\n";

            std::cout << "Current balance: "
                      << economy.getCoins()
                      << std::endl;
        }
    }

    previousBuyKeyState = buyKeyState;


            // -----------------------------
            // MOUSE POSITION
            // -----------------------------

            double mouseX = 0.0;
            double mouseY = 0.0;

            glfwGetCursorPos(
                window,
                &mouseX,
                &mouseY
            );

            int windowWidth = 0;
            int windowHeight = 0;

            glfwGetWindowSize(
                window,
                &windowWidth,
                &windowHeight
            );

            int hoveredLane = -1;

            if (windowWidth > 0 &&
                windowHeight > 0) {

                float normalizedX =
                    static_cast<float>(mouseX / windowWidth);

                float normalizedY =
                    static_cast<float>(mouseY / windowHeight);

                hoveredLane = grid.getLaneAtPosition(
                    normalizedX,
                    normalizedY
                );
            }

            // -----------------------------
            // GRID CLICK DETECTION
            // -----------------------------

            int leftMouseState = glfwGetMouseButton(
                window,
                GLFW_MOUSE_BUTTON_LEFT
            );

            if (leftMouseState == GLFW_RELEASE) {
                suppressGameplayClick = false;
            }

            if (!suppressGameplayClick &&
                leftMouseState == GLFW_PRESS &&
                previousLeftMouseState == GLFW_RELEASE &&
                hoveredLane != -1) {

                grid.toggleLaneSelection(hoveredLane);

                std::cout << "Lane "
                          << hoveredLane
                          << " selection toggled!\n";
            }

            previousLeftMouseState = leftMouseState;

            // -----------------------------
            // HOVER DETECTION
            // -----------------------------

            if (hoveredLane != previousHoveredLane) {

                if (hoveredLane != -1) {
                    std::cout
                        << "Hovering over Lane "
                        << hoveredLane
                        << '\n';
                }
                else {
                    std::cout
                        << "Mouse outside lanes\n";
                }

                previousHoveredLane = hoveredLane;
            }

            // -----------------------------
            // UPDATE SHADER UNIFORMS
            // -----------------------------

            glUseProgram(program);

            const auto& lanes = grid.getLanes();

            float bounds[16];

            for (int i = 0; i < 4; i++) {
                const Lane& lane = lanes[i];

                bounds[i * 4 + 0] = lane.left;
                bounds[i * 4 + 1] = lane.right;
                bounds[i * 4 + 2] = lane.top;
                bounds[i * 4 + 3] = lane.bottom;
            }

            glUniform4fv(
                glGetUniformLocation(
                    program,
                    "laneBounds[0]"
                ),
                4,
                bounds
            );

            glUniform4f(
                glGetUniformLocation(
                    program,
                    "selectedFlags"
                ),
                lanes[0].selected ? 1.0f : 0.0f,
                lanes[1].selected ? 1.0f : 0.0f,
                lanes[2].selected ? 1.0f : 0.0f,
                lanes[3].selected ? 1.0f : 0.0f
            );

            glUniform1i(
                glGetUniformLocation(
                    program,
                    "hoveredLane"
                ),
                hoveredLane
            );

            glUniform1i(
                glGetUniformLocation(
                    program,
                    "backgroundTexture"
                ),
                0
            );

            // -----------------------------
            // DRAW BACKGROUND
            // -----------------------------

            glActiveTexture(GL_TEXTURE0);

            glBindTexture(
                GL_TEXTURE_2D,
                backgroundTexture
            );

            glBindVertexArray(vao);

            glDrawArrays(
                GL_TRIANGLES,
                0,
                6
            );

            glBindVertexArray(0);

            // -----------------------------
            // TEAMMATE'S GAME LOGIC
            // -----------------------------

            game.update(deltaTime);
            game.render(grid.getLanes()[playerLaneId]);
        }

        // Future: render cutscene here when available.

        glfwSwapBuffers(window);
    }

    // =====================================
    // CLEANUP
    // =====================================

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(program);
    glDeleteTextures(1, &backgroundTexture);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
