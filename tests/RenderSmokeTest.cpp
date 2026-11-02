#define GLFW_INCLUDE_NONE
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "Game.h"
#include "entities/MeleePlayer.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// Save a normal BMP for visual review without adding an image library.
std::vector<unsigned char> saveFrame(const std::string& filename) {
    const int width = 1000, height = 700;
    std::vector<unsigned char> pixels(width * height * 3);
    glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, pixels.data());
    unsigned char header[54] = {};
    header[0] = 'B'; header[1] = 'M';
    auto number = [&header](int offset, int value) {
        for (int i = 0; i < 4; ++i) header[offset + i] = static_cast<unsigned char>(value >> (8 * i));
    };
    number(2, 54 + static_cast<int>(pixels.size()));
    number(10, 54); number(14, 40); number(18, width); number(22, height);
    header[26] = 1; header[28] = 24;
    std::ofstream file(filename, std::ios::binary);
    file.write(reinterpret_cast<const char*>(header), sizeof(header));
    file.write(reinterpret_cast<const char*>(pixels.data()), pixels.size());
    return pixels;
}
int main(int argc, char** argv) {
    if (!glfwInit()) { std::cerr << "GLFW init failed\n"; return 1; }
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(1000, 700, "Render verification", nullptr, nullptr);
    if (!window) { glfwTerminate(); return 1; }
    glfwMakeContextCurrent(window);
    if (!gladLoadGL(glfwGetProcAddress)) { glfwDestroyWindow(window); glfwTerminate(); return 1; }
    glViewport(0, 0, 1000, 700);
    bool success = true;
    {
        Game game;
        success = game.init();
        std::string prefix = argc > 1 ? argv[1] : "build-ucrt/";
        if (success) {
            glClear(GL_COLOR_BUFFER_BIT);
            game.update(2);
            game.render();
            auto opening = saveFrame(prefix + "intro-opening.bmp");
            game.update(6);
            game.render();
            auto battle = saveFrame(prefix + "intro-battle.bmp");
            success = opening != battle;
            game.skipCutscene();
            Player* melee = game.placePlayer(PlayerType::Melee, 0);
            Player* ranged = game.placePlayer(PlayerType::Ranged, 1);
            game.placePlayer(PlayerType::Defense, 2);
            melee->takeDamage(2);
            ranged->duck();
            game.getWaveManager().update(9);
            game.getWaveManager().getEnemyAt(0)->takeDamage(100);
            game.getWaveManager().finishUpdate();
            game.update(.2f);
            game.render();
            auto gameplay = saveFrame(prefix + "gameplay.bmp");
            game.update(.2f);
            game.render();
            success = success && gameplay != saveFrame(prefix + "gameplay-animated.bmp");
            HUD hud;
            glEnable(GL_SCISSOR_TEST);
            glScissor(11, 12, 30, 40);
            glClearColor(.1f, .2f, .3f, .4f);
            hud.drawhealth(100, 100, 50, 5, 3, 5);
            GLint box[4]; GLfloat color[4];
            glGetIntegerv(GL_SCISSOR_BOX, box);
            glGetFloatv(GL_COLOR_CLEAR_VALUE, color);
            success = success && glIsEnabled(GL_SCISSOR_TEST) && box[0] == 11 && box[2] == 30 && color[1] == .2f;
            glDisable(GL_SCISSOR_TEST);
            success = success && glGetError() == GL_NO_ERROR;
        }
        game.cleanup();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    std::cout << (success ? "Render verification passed\n" : "Render verification failed\n");
    return success ? 0 : 1;
}
