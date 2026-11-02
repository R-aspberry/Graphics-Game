#define GLFW_INCLUDE_NONE
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#ifdef _WIN32
#include <GLFW/glfw3native.h>
#include <dwmapi.h>
#endif
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <string>
#include "Game.h"
#include "ui/MainMenu.h"

// The existing menu artwork and button positions are unchanged.
enum class AppScreen { MENU, CUTSCENE, PLAYING, WON, LOST };
static AppScreen currentScreen = AppScreen::MENU;
static bool startRequested = false;
static bool suppressGameplayClick = false;

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    (void)mods;
    if (button != GLFW_MOUSE_BUTTON_LEFT || action != GLFW_PRESS ||
        currentScreen != AppScreen::MENU) return;
    double mouseX = 0, mouseY = 0;
    int width = 0, height = 0;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    glfwGetWindowSize(window, &width, &height);
    if (width <= 0 || height <= 0) return;
    double x = 2.0 * mouseX / width - 1.0;
    double y = 1.0 - 2.0 * mouseY / height;
    if (x >= -.46 && x <= -.10 && y >= 0 && y <= .20) {
        startRequested = true;
        suppressGameplayClick = true;
    } else if (x >= .10 && x <= .46 && y >= 0 && y <= .20) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void customizeTitleBar(GLFWwindow* window) {
#ifdef _WIN32
    HWND hwnd = glfwGetWin32Window(window);
    COLORREF background = RGB(229, 231, 235);
    COLORREF textColor = RGB(17, 24, 39);
    BOOL darkMode = FALSE;
    DwmSetWindowAttribute(hwnd, 20, &darkMode, sizeof(darkMode));
    DwmSetWindowAttribute(hwnd, 35, &background, sizeof(background));
    DwmSetWindowAttribute(hwnd, 36, &textColor, sizeof(textColor));
#else
    (void)window;
#endif
}

void purchaseMelee(int laneSelected, Game* game) {
    game->purchasePlayer(PlayerType::Melee, laneSelected);
}
void purchaseRanged(int laneSelected, Game* game) {
    game->purchasePlayer(PlayerType::Ranged, laneSelected);
}
void purchaseDefense(int laneSelected, Game* game) {
    game->purchasePlayer(PlayerType::Defense, laneSelected);
}

// CMake copies assets beside the executable, so launching from another folder works.
void findAssets(const char* executableName) {
    std::error_code error;
    std::filesystem::path executable;
#ifdef _WIN32
    wchar_t filename[32768] = {};
    DWORD length = GetModuleFileNameW(nullptr, filename, 32768);
    if (length > 0 && length < 32768) executable = std::filesystem::path(filename);
#else
    executable = std::filesystem::absolute(executableName, error);
#endif
    if (!executable.empty() && std::filesystem::is_directory(executable.parent_path() / "assets", error))
        std::filesystem::current_path(executable.parent_path(), error);
}

int main(int argc, char** argv) {
    findAssets(argv[0]);
    // A hidden, short launch check exercises the real application without leaving a window open.
    bool smokeTest = argc > 1 && std::string(argv[1]) == "--smoke-test";
    if (!glfwInit()) { std::cerr << "Failed to initialize GLFW\n"; return 1; }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif
    if (smokeTest) glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(1000, 700, "Tower Defense Game", nullptr, nullptr);
    if (!window) { std::cerr << "Failed to create window\n"; glfwTerminate(); return 1; }
    customizeTitleBar(window);
    glfwMakeContextCurrent(window);
    // Keep short presses until the frame reads them, including fast mouse clicks.
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
    glfwSwapInterval(smokeTest ? 0 : 1);
    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }
    std::cout << "OpenGL " << glGetString(GL_VERSION) << " initialized\n";

    MainMenu mainMenu;
    mainMenu.init();
    Game game;
    bool gameInitialized = false;
    Grid& grid = game.getGrid();
    int laneSelected = -1;
    bool isPaused = false;
    int previousLeftMouseState = GLFW_RELEASE;
    int previousEscapeState = GLFW_RELEASE;
    int previousBuyKeyState = GLFW_RELEASE;
    int previousJumpState = GLFW_RELEASE;
    int previousDuckState = GLFW_RELEASE;
    int previousSkipState = GLFW_RELEASE;
    double lastFrameTime = glfwGetTime();
    int exitCode = 0;
    int frameCount = 0;
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    startRequested = false;

    auto restartGame = [&]() {
        game.reset();
        laneSelected = -1;
        isPaused = false;
        currentScreen = AppScreen::PLAYING;
        suppressGameplayClick = true;
        glfwSetWindowTitle(window, "Tower Defense Game");
    };

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        double now = glfwGetTime();
        // Ignore long stalls while resizing or switching windows.
        float deltaTime = static_cast<float>(std::min(now - lastFrameTime, .1));
        lastFrameTime = now;
        if (startRequested) {
            startRequested = false;
            if (!gameInitialized) gameInitialized = game.init();
            if (!gameInitialized) {
                std::cerr << "Game initialization failed. Check the assets folder.\n";
                exitCode = 1;
                break;
            }
            game.reset();
            game.startCutscene();
            laneSelected = -1;
            isPaused = false;
            currentScreen = AppScreen::CUTSCENE;
            glfwSetWindowTitle(window, "Tower Defense Game");
            lastFrameTime = glfwGetTime();
            deltaTime = 0;
        }

        int framebufferWidth = 0, framebufferHeight = 0;
        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
        if (framebufferWidth <= 0 || framebufferHeight <= 0) {
            glfwWaitEventsTimeout(.05);
            continue;
        }
        glViewport(0, 0, framebufferWidth, framebufferHeight);
        glClearColor(.1f, .1f, .15f, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        double mouseX = 0, mouseY = 0;
        int windowWidth = 0, windowHeight = 0;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        float normalizedX = windowWidth > 0 ? static_cast<float>(mouseX / windowWidth) : 0;
        float normalizedY = windowHeight > 0 ? static_cast<float>(mouseY / windowHeight) : 0;
        double x = 2.0 * normalizedX - 1.0;
        double y = 1.0 - 2.0 * normalizedY;
        int leftMouseState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        if (leftMouseState == GLFW_RELEASE) suppressGameplayClick = false;
        bool clicked = !suppressGameplayClick && leftMouseState == GLFW_PRESS &&
                       previousLeftMouseState == GLFW_RELEASE;
        int escapeState = glfwGetKey(window, GLFW_KEY_ESCAPE);
        int buyKeyState = glfwGetKey(window, GLFW_KEY_M);
        int jumpState = (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ||
                         glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) ? GLFW_PRESS : GLFW_RELEASE;
        int duckState = (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ||
                         glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) ? GLFW_PRESS : GLFW_RELEASE;
        int skipState = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS ||
                         glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) ? GLFW_PRESS : GLFW_RELEASE;

        if (currentScreen == AppScreen::MENU) {
            mainMenu.draw();
        } else if (currentScreen == AppScreen::CUTSCENE) {
            if ((skipState == GLFW_PRESS && previousSkipState == GLFW_RELEASE) ||
                (smokeTest && frameCount >= 3)) game.skipCutscene();
            game.update(deltaTime);
            game.render();
            if (!game.isCutscenePlaying()) {
                currentScreen = AppScreen::PLAYING;
                suppressGameplayClick = true;
            }
        } else if (currentScreen == AppScreen::PLAYING) {
            if (escapeState == GLFW_PRESS && previousEscapeState == GLFW_RELEASE) {
                isPaused = !isPaused;
                suppressGameplayClick = true;
                clicked = false;
                glfwSetWindowTitle(window, isPaused ? "Tower Defense Game - PAUSED" : "Tower Defense Game");
            }
            int hoveredLane = -1, hoveredCard = -1;
            if (isPaused) {
                if (clicked && x >= -.24 && x <= .24) {
                    if (y >= .20 && y <= .48) {
                        isPaused = false;
                        suppressGameplayClick = true;
                        glfwSetWindowTitle(window, "Tower Defense Game");
                    } else if (y >= -.14 && y <= .14) {
                        restartGame();
                    } else if (y >= -.48 && y <= -.20) {
                        currentScreen = AppScreen::MENU;
                        isPaused = false;
                        suppressGameplayClick = true;
                        glfwSetWindowTitle(window, "Tower Defense Game");
                    }
                }
            } else {
                hoveredLane = grid.getLaneAtPosition(normalizedX, normalizedY);
                if (y >= .55 && y <= .95) {
                    if (x >= -.90 && x <= -.68) hoveredCard = 0;
                    else if (x >= -.62 && x <= -.40) hoveredCard = 1;
                    else if (x >= -.34 && x <= -.12) hoveredCard = 2;
                }
                if (clicked && hoveredLane != -1) {
                    grid.toggleLaneSelection(hoveredLane);
                    if (grid.getLanes()[hoveredLane].selected) laneSelected = hoveredLane;
                    else if (laneSelected == hoveredLane) {
                        laneSelected = -1;
                        for (const Lane& lane : grid.getLanes())
                            if (lane.selected) { laneSelected = lane.id; break; }
                    }
                }
                if (clicked && hoveredCard == 0) purchaseDefense(laneSelected, &game);
                if (clicked && hoveredCard == 1) purchaseMelee(laneSelected, &game);
                if (clicked && hoveredCard == 2) purchaseRanged(laneSelected, &game);
                if (buyKeyState == GLFW_PRESS && previousBuyKeyState == GLFW_RELEASE)
                    purchaseMelee(laneSelected, &game);
                if (jumpState == GLFW_PRESS && previousJumpState == GLFW_RELEASE) game.jumpSelectedLanes();
                if (duckState == GLFW_PRESS && previousDuckState == GLFW_RELEASE) game.duckSelectedLanes();
            }
            // Game advances its own players, enemies, combat and economy exactly once.
            if (currentScreen == AppScreen::PLAYING) {
                game.setPaused(isPaused);
                game.update(deltaTime);
                game.render(hoveredLane, hoveredCard);
                if (game.hasWon() || game.hasLost()) {
                    currentScreen = game.hasWon() ? AppScreen::WON : AppScreen::LOST;
                    isPaused = false;
                    suppressGameplayClick = true;
                    clicked = false;
                    glfwSetWindowTitle(window, game.hasWon() ? "Tower Defense Game - YOU WON!" :
                                                            "Tower Defense Game - YOU LOST!");
                }
                if (isPaused) {
                    glEnable(GL_SCISSOR_TEST);
                    glScissor(framebufferWidth * 3 / 10, framebufferHeight / 5,
                              framebufferWidth * 4 / 10, framebufferHeight * 3 / 5);
                    glClearColor(.08f, .04f, .15f, 1);
                    glClear(GL_COLOR_BUFFER_BIT);
                    glDisable(GL_SCISSOR_TEST);
                    mainMenu.escTab();
                }
            } else {
                mainMenu.draw();
            }
        }

        if (currentScreen == AppScreen::WON || currentScreen == AppScreen::LOST) {
            game.render();
            mainMenu.endMenu(currentScreen == AppScreen::WON);
            if (clicked && x >= -.24 && x <= .24) {
                if (y >= -.14 && y <= .14) restartGame();
                else if (y >= -.48 && y <= -.20) {
                    currentScreen = AppScreen::MENU;
                    suppressGameplayClick = true;
                    glfwSetWindowTitle(window, "Tower Defense Game");
                }
            }
        }
        previousLeftMouseState = leftMouseState;
        previousEscapeState = escapeState;
        previousBuyKeyState = buyKeyState;
        previousJumpState = jumpState;
        previousDuckState = duckState;
        previousSkipState = skipState;
        if (smokeTest && glGetError() != GL_NO_ERROR) { exitCode = 1; break; }
        glfwSwapBuffers(window);
        if (smokeTest) {
            ++frameCount;
            if (frameCount == 1) startRequested = true;
            if (frameCount >= 12) glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    }
    game.cleanup();
    mainMenu.cleanup();
    glfwDestroyWindow(window);
    glfwTerminate();
    if (smokeTest && exitCode == 0) std::cout << "Application smoke test passed\n";
    return exitCode;
}
