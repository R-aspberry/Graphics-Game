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
#include <iostream>
#include <vector>
#include <memory>

#include "systems/SoundManager.h"
#include "ui/SoundSettings.h"
#include "../external/stb/stb_image.h"
#include "world/Grid.h"
#include "world/HomeBase.h"
#include "ui/MainMenu.h"
#include "Game.h"
#include "utils/ResourceManager.h"
#include "utils/Constants.h"
#include "systems/Economy.h"
#include "systems/WaveManager.h"
#include "ui/PlayerCard.h"
#include "ui/HUD.h"
#include "cutscene/Cutscene.h"

static const char *vertexShaderSource = R"(
    #version 330 core
    layout(location = 0) in vec3 aPos;
    layout(location = 1) in vec2 aTexCoord;
    out vec2 TexCoord;
    uniform bool isOverlay;
    uniform vec4 overlayRect;
    void main() {
        if (isOverlay) {
            vec2 position = overlayRect.xy + ((aPos.xy + 1.0) * 0.5) * overlayRect.zw;
            gl_Position = vec4(position, aPos.z, 1.0);
        } else {
            gl_Position = vec4(aPos, 1.0);
        }
        TexCoord = aTexCoord;
    }
)";

static const char *fragmentShaderSource = R"(
    #version 330 core
    in vec2 TexCoord;
    out vec4 FragColor;
    uniform sampler2D backgroundTexture;
    uniform vec4 laneBounds[4];
    uniform vec4 selectedFlags;
    uniform int hoveredLane;
    uniform bool isOverlay;
    void main() {
        vec4 color = texture(backgroundTexture, TexCoord);
        if (!isOverlay) {
            float x = TexCoord.x;
            float y = 1.0 - TexCoord.y;
            for (int i = 0; i < 4; ++i) {
                vec4 bounds = laneBounds[i];
                if (x >= bounds.x && x <= bounds.y &&
                    y >= bounds.z && y <= bounds.w) {
                    if (selectedFlags[i] > 0.5) {
                        color.rgb = mix(color.rgb, vec3(0.6, 0.2, 1.0), 0.4);
                    } else if (hoveredLane == i) {
                        color.rgb = mix(color.rgb, vec3(0.0, 0.9, 1.0), 0.3);
                    }
                }
            }
        }
        FragColor = color;
    }
)";

GLuint placeEl(const char *path)
{

    int imageWidth = 0;
    int imageHeight = 0;
    int channels = 0;

    stbi_set_flip_vertically_on_load(true);

    unsigned char *pixels = stbi_load(path, &imageWidth, &imageHeight, &channels, 4);
    GLuint backgroundTexture = 0;

    glGenTextures(1, &backgroundTexture);
    glBindTexture(GL_TEXTURE_2D, backgroundTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(pixels);

    return backgroundTexture;
}

static GLuint compileShader(GLenum kind, const char *source)
{
    const GLuint shader = glCreateShader(kind);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        char log[1024] = {};
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        std::cerr << "Shader compilation failed: " << log << '\n';
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

enum class AppScreen
{
    MENU,
    SOUND_SETTINGS,
    CUTSCENE,
    PLAYING,
    WON,
    LOST
};

static AppScreen currentScreen = AppScreen::MENU;

static bool suppressGameplayClick = false;
static bool startNewGameRequested = false;

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    (void)mods;

    if (button != GLFW_MOUSE_BUTTON_LEFT || action != GLFW_PRESS)
    {
        return;
    }

    if (currentScreen != AppScreen::MENU)
    {
        return;
    }

    double mouseX = 0.0;
    double mouseY = 0.0;
    int width = 0;
    int height = 0;

    glfwGetCursorPos(window, &mouseX, &mouseY);

    glfwGetWindowSize(window, &width, &height);

    if (width <= 0 || height <= 0)
    {
        return;
    }

    double x = (2.0 * mouseX / width) - 1.0;
    double y = 1.0 - (2.0 * mouseY / height);

    // START BUTTON

    if (x >= -0.46 && x <= -0.10 && y >= 0.00 && y <= 0.20)
    {
        currentScreen = AppScreen::CUTSCENE;
        suppressGameplayClick = true;
        startNewGameRequested = true;
    }

    // EXIT BUTTON

    else if (x >= 0.10 && x <= 0.46 && y >= 0.00 && y <= 0.20)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    else if (x >= -0.24 && x <= 0.24 && y >= -0.32 && y <= -0.12)
    {
        currentScreen = AppScreen::SOUND_SETTINGS;
    }
}

void customizeTitleBar(GLFWwindow *window)
{
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

bool purchasePlayer(int laneSelected, PlayerType type, int cost, Economy &economy, Game &game)
{
    if (laneSelected < 0 || laneSelected >= 4 || !economy.canAfford(cost))
    {
        return false;
    }
    if (game.placePlayer(type, laneSelected) == nullptr)
    {
        return false;
    }
    return economy.spendCoins(cost);
}

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *videoMode = nullptr;
    if (monitor != nullptr)
    {
        videoMode = glfwGetVideoMode(monitor);
    }

    int initialWindowWidth = 1000;
    int initialWindowHeight = 700;

    if (videoMode != nullptr)
    {
        initialWindowWidth = videoMode->width;
        initialWindowHeight = videoMode->height;
    }

    GLFWwindow *window = glfwCreateWindow(initialWindowWidth, initialWindowHeight, "Tower Defense Game", monitor, nullptr);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    GLFWcursor *cursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    glfwSetCursor(window, cursor);

    customizeTitleBar(window);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    // Retain a quick key press until the frame loop reads it.
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

    gladLoadGL(glfwGetProcAddress);

    SoundManager &audio = SoundManager::instance();

    if (audio.init())
    {
        audio.playMusic("assets/backgroundsound.wav", true);
    }
    std::cout << "OpenGL context initialized\n";
    MainMenu mainMenu;
    mainMenu.init();
    SoundSettings soundSettings;
    soundSettings.init();
    PlayerCard playerCard;
    playerCard.init();
    HUD hud;
    hud.init();
    Grid grid;
    std::unique_ptr<Game> game = std::make_unique<Game>();
    game->setGrid(&grid);
    game->init();

    // Play the cinematic between Start and actual gameplay.
    Cutscene cutscene;
    const bool cutsceneReady = cutscene.init();

    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    GLuint vert = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint frag = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    GLuint program = glCreateProgram();

    glAttachShader(program, vert);
    glAttachShader(program, frag);

    glLinkProgram(program);

    GLint linked = 0;

    glGetProgramiv(program, GL_LINK_STATUS, &linked);

    glDeleteShader(vert);
    glDeleteShader(frag);

    const float vertices[] = {
        // X       Y      Z      U      V

        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,

        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f};

    GLuint vao = 0;
    GLuint vbo = 0;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));

    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    int imageWidth = 0;
    int imageHeight = 0;
    int channels = 0;

    stbi_set_flip_vertically_on_load(true);

    unsigned char *pixels = stbi_load("assets/backgrounds/example2.png", &imageWidth, &imageHeight, &channels, 4);
    //e7na delwa2ty han7ot 5 elements
    GLuint bill = placeEl("assets/elements/Billboard.png");
    GLuint build = placeEl("assets/elements/building.png");
    GLuint camera = placeEl("assets/elements/camera.png");
    GLuint flags = placeEl("assets/elements/flags.png");
    GLuint lifter = placeEl("assets/elements/lifter.png");

    GLuint backgroundTexture = 0;

    glGenTextures(1, &backgroundTexture);
    glBindTexture(GL_TEXTURE_2D, backgroundTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(pixels);

    int laneSelected = -1;
    bool defencePLaced = false;
    Economy economy;
    WaveManager waveManager;
    HomeBase homeBase;
    double lastFrameTime = glfwGetTime();
    bool isPaused = false;
    int previousEscapeState = GLFW_RELEASE;
    int previousUpState = GLFW_RELEASE;
    int previousDownState = GLFW_RELEASE;
    int previousLeftMouseState = GLFW_RELEASE;
    int hoveredLane = -1;
    int hoveredCard = -1;
    int collectedCoins = 0;
    float gameplayTime = 0.0f;

    auto restartGame = [&]()
    {
        economy = Economy();
        homeBase.reset();
        waveManager = WaveManager();
        game->reset();
        collectedCoins = 0;
        gameplayTime = 0.0f;
        laneSelected = -1;
        defencePLaced = false;
        hoveredLane = -1;
        hoveredCard = -1;
        isPaused = false;
        previousUpState = GLFW_RELEASE;
        previousDownState = GLFW_RELEASE;
        suppressGameplayClick = true;
        glfwSetWindowTitle(window, "Tower Defense Game");
        for (int i = 0; i < 4; ++i)
        {
            if (grid.getLanes()[i].selected)
            {
                grid.toggleLaneSelection(i);
            }
        }
    };

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        if (startNewGameRequested)
        {
            restartGame();
            if (cutsceneReady)
                cutscene.start();
            else
                currentScreen = AppScreen::PLAYING;
            startNewGameRequested = false;
        }
        const double currentTime = glfwGetTime();
        const float deltaTime = std::min(
            static_cast<float>(currentTime - lastFrameTime),
            0.1f);
        lastFrameTime = currentTime;

        const int escapeState = glfwGetKey(window, GLFW_KEY_ESCAPE);
        if (currentScreen == AppScreen::PLAYING && escapeState == GLFW_PRESS && previousEscapeState == GLFW_RELEASE)
        {
            isPaused = !isPaused;
            suppressGameplayClick = true;
            if (isPaused)
            {
                glfwSetWindowTitle(window, "Tower Defense Game - PAUSED");
            }
            else
            {
                glfwSetWindowTitle(window, "Tower Defense Game");
            }
        }
        previousEscapeState = escapeState;

        int framebufferWidth = 0;
        int framebufferHeight = 0;

        glfwGetFramebufferSize(
            window,
            &framebufferWidth,
            &framebufferHeight);

        if (framebufferWidth <= 0 || framebufferHeight <= 0)
        {
            glfwSwapBuffers(window);
            continue;
        }

        glViewport(0, 0, framebufferWidth, framebufferHeight);
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT);

        if (currentScreen == AppScreen::MENU)
        {
            mainMenu.draw();
        }

        else if (currentScreen == AppScreen::SOUND_SETTINGS)
        {
            soundSettings.draw();

            double mx, my;
            glfwGetCursorPos(window, &mx, &my);

            int width, height;
            glfwGetWindowSize(window, &width, &height);

            int mouseState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

            if (width > 0 && height > 0 && mouseState == GLFW_PRESS && previousLeftMouseState == GLFW_RELEASE)
            {
                float x = static_cast<float>(2.0 * mx / width - 1.0);
                float y = static_cast<float>(1.0 - 2.0 * my / height);

                SoundAction action = soundSettings.handleClick(x, y);

                if (action == SoundAction::Back)
                {
                    currentScreen = AppScreen::MENU;
                }
                else if (action == SoundAction::VolumeUp)
                {
                    audio.setVolume(audio.getVolume() + 0.1f);
                }
                else if (action == SoundAction::VolumeDown)
                {
                    audio.setVolume(audio.getVolume() - 0.1f);
                }
                else if (action == SoundAction::Mute)
                {
                    audio.toggleMute();
                }
            }

            previousLeftMouseState = mouseState;
        }
        else
        {
            if (currentScreen == AppScreen::CUTSCENE)
            {
                cutscene.update(deltaTime);
                if (cutscene.finished() || glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
                {
                    cutscene.stopAudio();
                    currentScreen = AppScreen::PLAYING;
                    suppressGameplayClick = true;
                }
                else
                {
                    cutscene.draw(framebufferWidth, framebufferHeight);
                }
            }
            double mouseX = 0.0;
            double mouseY = 0.0;
            glfwGetCursorPos(window, &mouseX, &mouseY);
            int windowWidth = 0;
            int windowHeight = 0;
            glfwGetWindowSize(window, &windowWidth, &windowHeight);
            const int leftMouseState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
            const bool click = leftMouseState == GLFW_PRESS && previousLeftMouseState == GLFW_RELEASE;
            if (leftMouseState == GLFW_RELEASE)
            {
                suppressGameplayClick = false;
            }

            if (currentScreen == AppScreen::PLAYING)
            {
                if (!isPaused && windowWidth > 0 && windowHeight > 0)
                {
                    hoveredLane = grid.getLaneAtPosition(static_cast<float>(mouseX / windowWidth), static_cast<float>(mouseY / windowHeight));
                    const float x = static_cast<float>(2.0 * mouseX / windowWidth - 1.0);
                    const float y = static_cast<float>(1.0 - 2.0 * mouseY / windowHeight);
                    hoveredCard = -1;
                    if (y >= 0.55f && y <= 0.95f)
                    {
                        if (x >= -0.90f && x <= -0.68f)
                            hoveredCard = 0;
                        else if (x >= -0.62f && x <= -0.40f)
                            hoveredCard = 1;
                        else if (x >= -0.34f && x <= -0.12f)
                            hoveredCard = 2;
                    }
                }

                if (!isPaused && click && !suppressGameplayClick)
                {
                    if (hoveredLane >= 0)
                    {
                        if (defencePLaced && economy.canAfford(30))
                        {
                            float clickedX = (float)mouseX / windowWidth;
                            Player *defence = game->placePlayer(PlayerType::Defense, hoveredLane);

                            if (defence != nullptr)
                            {
                                float x = clickedX - (defence->getWidth() / 2);
                                const Lane &lane = grid.getLanes()[hoveredLane];
                                float maxright = lane.right - defence->getWidth();
                                if (x < lane.left)
                                {
                                    x = lane.left;
                                }
                                else if (x > maxright)
                                {
                                    x = maxright;
                                }
                                defence->setPosition(x, defence->getY());
                                economy.spendCoins(30);
                                defencePLaced = false;
                            }
                        }
                        else
                        {
                            grid.toggleLaneSelection(hoveredLane);
                            laneSelected = -1;
                        }
                    }
                    else if (hoveredCard >= 0)
                    {
                        PlayerType type = PlayerType::Ranged;
                        int cost = 20;
                        if (hoveredCard == 0)
                        {
                            type = PlayerType::Defense;
                            cost = 30;
                        }
                        else if (hoveredCard == 1)
                        {
                            type = PlayerType::Melee;
                            cost = 10;
                        }
                        const bool unlocked = hoveredCard == 1 ||
                                              (hoveredCard == 2 && collectedCoins >= 20 && economy.canAfford(cost)) ||
                                              (hoveredCard == 0 && collectedCoins >= 30 && economy.canAfford(cost));
                        if (unlocked)
                        {
                            if (hoveredCard == 0)
                            {
                                defencePLaced = true; // one defense only placed at a time not more we care about mouse click not lanes selected
                            }
                            else
                            {
                                for (const auto &lane : grid.getLanes())
                                {
                                    if (lane.selected)
                                    {
                                        laneSelected = lane.id;
                                        purchasePlayer(laneSelected, type, cost, economy, *game);
                                    }
                                }
                            }
                        }
                    }
                }

                if (!isPaused)
                {
                    gameplayTime += deltaTime;
                    const int upState =
                        glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ||
                        glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS;
                    const int downState =
                        glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ||
                        glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS;
                    if (upState && previousUpState == GLFW_RELEASE)
                    {
                        game->jumpPlayers();
                    }
                    if (downState && previousDownState == GLFW_RELEASE)
                    {
                        game->duckPlayers();
                    }
                    if (upState)
                    {
                        previousUpState = GLFW_PRESS;
                    }
                    else
                    {
                        previousUpState = GLFW_RELEASE;
                    }
                    if (downState)
                    {
                        previousDownState = GLFW_PRESS;
                    }
                    else
                    {
                        previousDownState = GLFW_RELEASE;
                    }

                    waveManager.update(deltaTime);
                    std::vector<EnemyProjectile *> shots;

                    for (size_t i = 0; i < waveManager.getEnemyProjectilesCount(); i++)
                    {
                        shots.push_back(waveManager.getEnemyProjectileAt(i));
                    }

                    // Check bullet collisions
                    game->getCombatSystem().resolveEnemyProjectiles(shots);
                    std::vector<Enemy *> enemies;
                    enemies.reserve(waveManager.getActiveEnemyCount());
                    for (std::size_t i = 0; i < waveManager.getActiveEnemyCount(); ++i)
                    {
                        if (Enemy *enemy = waveManager.getEnemyAt(i))
                        {
                            enemies.push_back(enemy);
                        }
                    }
                    game->update(deltaTime, enemies);

                    for (std::size_t i = waveManager.getDropCount(); i > 0; --i)
                    {
                        Collectible *drop = waveManager.getDropAt(i - 1);
                        if (drop == nullptr)
                            continue;
                        bool collected = false;
                        for (Player *player : game->getPlayersInLane(drop->getLane()))
                        {
                            if (player == nullptr || !player->isAlive())
                                continue;
                            const bool overlaps =
                                player->getX() < drop->getX() + drop->getWidth() &&
                                player->getX() + player->getWidth() > drop->getX() &&
                                player->getY() < drop->getY() + drop->getHeight() &&
                                player->getY() + player->getHeight() > drop->getY();
                            if (overlaps)
                            {
                                collected = drop->collect();
                                break;
                            }
                        }
                        if (collected)
                        {
                            const int reward = drop->getValueOfCoins();
                            economy.addCoins(reward);
                            collectedCoins += reward;
                            waveManager.removeDropAt(i - 1);
                        }
                    }

                    if (waveManager.hasEnemyReachedBase(WorldLayout::HOME_BASE_X))
                    {
                        homeBase.enemyReached();
                    }
                    if (homeBase.isDestroyed())
                    {
                        currentScreen = AppScreen::LOST;
                        isPaused = false;
                        glfwSetWindowTitle(window, "Tower Defense Game - YOU LOST!");
                    }
                    else if (waveManager.hasWon())
                    {
                        currentScreen = AppScreen::WON;
                        isPaused = false;

                        audio.playMusic("assets/samoray.wav", false);

                        glfwSetWindowTitle(window, "Tower Defense Game - YOU WON!");
                    }
                }

                if (click && isPaused && windowWidth > 0 && windowHeight > 0)
                {
                    const double x = 2.0 * mouseX / windowWidth - 1.0;
                    const double y = 1.0 - 2.0 * mouseY / windowHeight;
                    if (x >= -0.24 && x <= 0.24 && y >= 0.20 && y <= 0.48)
                    {
                        isPaused = false;
                        glfwSetWindowTitle(window, "Tower Defense Game");
                        suppressGameplayClick = true;
                    }
                    else if (x >= -0.24 && x <= 0.24 && y >= -0.48 && y <= -0.20)
                    {
                        currentScreen = AppScreen::MENU;
                        isPaused = false;
                        glfwSetWindowTitle(window, "Tower Defense Game");
                        suppressGameplayClick = true;
                    }
                    else if (x >= -0.24 && x <= 0.24 && y >= -0.14 && y <= 0.14)
                    {
                        restartGame();
                        currentScreen = AppScreen::PLAYING;
                        isPaused = false;
                        suppressGameplayClick = true;
                    }
                }
            }

            if (currentScreen == AppScreen::PLAYING)
            {
                glUseProgram(program);
                const auto &lanes = grid.getLanes();
                float bounds[16];
                for (int i = 0; i < 4; ++i)
                {
                    bounds[i * 4] = lanes[i].left;
                    bounds[i * 4 + 1] = lanes[i].right;
                    bounds[i * 4 + 2] = lanes[i].top;
                    bounds[i * 4 + 3] = lanes[i].bottom;
                }
                glUniform4fv(glGetUniformLocation(program, "laneBounds[0]"), 4, bounds);
                float selectedFlags[4] = {};
                for (int i = 0; i < 4; ++i)
                {
                    if (lanes[i].selected)
                    {
                        selectedFlags[i] = 1.0f;
                    }
                }
                glUniform4fv(glGetUniformLocation(program, "selectedFlags"), 1, selectedFlags);
                int visibleHoveredLane = -1;
                if (currentScreen == AppScreen::PLAYING && !isPaused)
                {
                    visibleHoveredLane = hoveredLane;
                }
                glUniform1i(glGetUniformLocation(program, "hoveredLane"), visibleHoveredLane);
                glUniform1i(glGetUniformLocation(program, "isOverlay"), 0);
                glUniform1i(glGetUniformLocation(program, "backgroundTexture"), 0);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, backgroundTexture);
                glBindVertexArray(vao);
                glDrawArrays(GL_TRIANGLES, 0, 6);
                glBindVertexArray(0);

                const auto drawElement = [&](GLuint texture, float x, float y, float width, float height)
                {
                    glUniform1i(glGetUniformLocation(program, "isOverlay"), 1);
                    glUniform4f(glGetUniformLocation(program, "overlayRect"),
                                x, y, width, height);
                    glBindTexture(GL_TEXTURE_2D, texture);
                    glBindVertexArray(vao);
                    glDrawArrays(GL_TRIANGLES, 0, 6);
                };
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                drawElement(build, -0.62f, 0.05f, 0.34f, 0.58f);
                drawElement(bill, -0.36f, 0.05f, 0.48f, 0.24f);
                drawElement(camera, 0.69f, 0.05f, 0.18f, 0.36f);
                drawElement(flags, 0.43f, 0.05f, 0.20f, 0.40f);
                drawElement(lifter, 0.06f, 0.05f, 0.36f, 0.25f);
                glBindVertexArray(0);
                glUniform1i(glGetUniformLocation(program, "isOverlay"), 0);

                if (currentScreen == AppScreen::PLAYING)
                {
                    for (std::size_t i = 0; i < waveManager.getActiveEnemyCount(); ++i)
                    {
                        const Enemy *enemy = waveManager.getEnemyAt(i);
                        if (enemy != nullptr && enemy->getLane() >= 0 && enemy->getLane() < 4)
                        {
                            game->renderEnemy(*enemy, lanes[enemy->getLane()], gameplayTime);
                        }
                    }
                    game->render();
                    game->renderEnemyProjectiles(waveManager);
                    game->renderCollectibles(waveManager);

                    const bool meleeUnlocked = economy.canAfford(10);
                    const bool rangedUnlocked = collectedCoins >= 20 && economy.canAfford(20); // both checks needed because at first ranged and defense not unclocked
                    const bool defenseUnlocked = collectedCoins >= 30 && economy.canAfford(30);
                    int visibleHoveredCard = -1;
                    if (!isPaused)
                    {
                        visibleHoveredCard = hoveredCard;
                    }
                    playerCard.draw(visibleHoveredCard, defenseUnlocked, meleeUnlocked, rangedUnlocked);
                    hud.drawCoins(economy.getCoins(), framebufferWidth, framebufferHeight);
                    for (int lane = 0; lane < 4; ++lane)
                    {
                        for (Player *player : game->getPlayersInLane(lane))
                        {
                            if (player != nullptr && player->isAlive())
                            {
                                hud.drawhealth(static_cast<int>(player->getX() * framebufferWidth),
                                               static_cast<int>((1.0f - player->getY()) * framebufferHeight),
                                               38, 5, player->getHealth(), player->getMaxHealth());
                            }
                        }
                    }
                    for (std::size_t i = 0; i < waveManager.getActiveEnemyCount(); ++i)
                    {
                        const Enemy *enemy = waveManager.getEnemyAt(i);
                        if (enemy != nullptr && enemy->getState() != EnemyState::DEAD)
                        {
                            hud.drawhealth(static_cast<int>(enemy->getPosition().x * framebufferWidth),
                                           static_cast<int>((1.0f - enemy->getPosition().y) * framebufferHeight),
                                           38, 5, enemy->getHealth(), enemy->getMaxHealth());
                        }
                    }
                    if (isPaused)
                        mainMenu.escTab();
                }
            }
            else if (currentScreen == AppScreen::WON || currentScreen == AppScreen::LOST)
            {
                mainMenu.endMenu(currentScreen == AppScreen::WON);
                if (click && !suppressGameplayClick && windowWidth > 0 && windowHeight > 0)
                {
                    const double x = 2.0 * mouseX / windowWidth - 1.0;
                    const double y = 1.0 - 2.0 * mouseY / windowHeight;
                    if (x >= -0.24 && x <= 0.24 && y >= -0.14 && y <= 0.14)
                    {
                        restartGame();
                        audio.playMusic("assets/backgroundsound.wav", true);
                        currentScreen = AppScreen::PLAYING;
                    }
                    else if (x >= -0.24 && x <= 0.24 && y >= -0.48 && y <= -0.20)
                    {
                        currentScreen = AppScreen::MENU;
                        glfwSetWindowTitle(window, "Tower Defense Game");
                        audio.playMusic("assets/backgroundsound.wav", true);
                    }
                    suppressGameplayClick = true;
                }
            }
            previousLeftMouseState = leftMouseState;
        }

        glfwSwapBuffers(window);
    }

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(program);
    glDeleteTextures(1, &backgroundTexture);
    const GLuint elementTextures[] = {bill, build, camera, flags, lifter};
    glDeleteTextures(5, elementTextures);

    cutscene.cleanup();
    audio.shutdown();
    game.reset();
    ResourceManager::clear();
    hud.cleanup();
    glfwSetMouseButtonCallback(window, nullptr);
    soundSettings.cleanup();
    glfwDestroyCursor(cursor);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
