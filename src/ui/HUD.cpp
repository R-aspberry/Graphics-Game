
#include "HUD.h"
#include <glad/gl.h>
#include <algorithm>
#include <string>

namespace
{
    // Draw a small colored rectangle in screen pixels.
    void drawRect(int x, int y, int width, int height,
                  float r, float g, float b)
    {
        glScissor(x, y, width, height);
        glClearColor(r, g, b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    // Draw a number using seven glowing segments.
    void drawDigit(int digit, int x, int y, int scale = 1)
    {
        const bool segments[10][7] = {
            {1, 1, 1, 0, 1, 1, 1}, // 0
            {0, 0, 1, 0, 0, 1, 0}, // 1
            {1, 0, 1, 1, 1, 0, 1}, // 2
            {1, 0, 1, 1, 0, 1, 1}, // 3
            {0, 1, 1, 1, 0, 1, 0}, // 4
            {1, 1, 0, 1, 0, 1, 1}, // 5
            {1, 1, 0, 1, 1, 1, 1}, // 6
            {1, 0, 1, 0, 0, 1, 0}, // 7
            {1, 1, 1, 1, 1, 1, 1}, // 8
            {1, 1, 1, 1, 0, 1, 1}  // 9
        };

        const int rects[7][4] = {
            {3, 27, 12, 3},
            {0, 16, 3, 11},
            {15, 16, 3, 11},
            {3, 14, 12, 3},
            {0, 3, 3, 11},
            {15, 3, 3, 11},
            {3, 0, 12, 3}};

        for (int i = 0; i < 7; i++)
        {
            if (segments[digit][i])
            {
                drawRect(
                    x + rects[i][0] * scale,
                    y + rects[i][1] * scale,
                    rects[i][2] * scale,
                    rects[i][3] * scale,
                    1.0f, 0.82f, 0.25f);
            }
        }
    }
}

void HUD::drawCoins(
    int coins,
    int framebufferWidth,
    int framebufferHeight)

{
    if (framebufferWidth < 220 ||
        framebufferHeight < 90)
    {
        return;
    }

    // Preserve existing OpenGL state.
    GLboolean scissorWasEnabled =
        glIsEnabled(GL_SCISSOR_TEST);

    GLint oldScissor[4];
    GLfloat oldClearColor[4];

    glGetIntegerv(GL_SCISSOR_BOX, oldScissor);
    glGetFloatv(GL_COLOR_CLEAR_VALUE, oldClearColor);

    glEnable(GL_SCISSOR_TEST);

    // Top-right HUD position
    const int scale = std::max(1, std::min(framebufferWidth / 1000, framebufferHeight / 700));
    const int x = framebufferWidth - 210 * scale;
    const int y = framebufferHeight - 70 * scale;

    // Dark background
    drawRect(x, y, 198 * scale, 58 * scale,
             0.07f, 0.05f, 0.16f);

    // Gold coin icon
    drawRect(x + 20 * scale, y + 15 * scale, 20 * scale, 30 * scale,
             0.95f, 0.65f, 0.12f);

    drawRect(x + 15 * scale, y + 20 * scale, 30 * scale, 20 * scale,
             0.95f, 0.65f, 0.12f);

    drawRect(x + 27 * scale, y + 22 * scale, 4 * scale, 16 * scale,
             0.60f, 0.35f, 0.06f);

    // Current balance
    std::string balance =
        std::to_string(std::max(0, coins));

    int digitX = x + 64 * scale;

    for (char character : balance)
    {
        drawDigit(character - '0',
                  digitX, y + 14 * scale, scale);

        digitX += 22 * scale;
    }

    // Restore OpenGL state.
    glScissor(
        oldScissor[0], oldScissor[1],
        oldScissor[2], oldScissor[3]);

    glClearColor(
        oldClearColor[0], oldClearColor[1],
        oldClearColor[2], oldClearColor[3]);

    if (!scissorWasEnabled)
    {
        glDisable(GL_SCISSOR_TEST);
    }
}




void HUD::drawhealth(int x,int y,int w, int h,int health,int maxHealth)
{
    if (maxHealth <= 0 || w <= 0 || h <= 0)
        return;
    if (health < 0)
        health = 0;
    if (health > maxHealth)
        health = maxHealth;
    float actualHealth = (health/(float)maxHealth)*w;
    GLboolean wasEnabled = glIsEnabled(GL_SCISSOR_TEST);
    GLint oldScissor[4];
    GLfloat oldColor[4];
    glGetIntegerv(GL_SCISSOR_BOX, oldScissor);
    glGetFloatv(GL_COLOR_CLEAR_VALUE, oldColor);
    glEnable(GL_SCISSOR_TEST);
    drawRect(x,y,w,h,0.2f,0.2f,0.2f);

    drawRect(x, y, (int)actualHealth, h, 0.0f, 1.0f, 0.0f);
    glScissor(oldScissor[0], oldScissor[1], oldScissor[2], oldScissor[3]);
    glClearColor(oldColor[0], oldColor[1], oldColor[2], oldColor[3]);
    if (!wasEnabled) glDisable(GL_SCISSOR_TEST);
}


void HUD::drawText(const char* text, int x, int y, int scale) {
    if (!text || scale <= 0) return;
    // Seven rows of five pixels per capital letter. No font dependency needed.
    static const unsigned char letters[26][7] = {
        {14,17,17,31,17,17,17}, {30,17,17,30,17,17,30},
        {14,17,16,16,16,17,14}, {30,17,17,17,17,17,30},
        {31,16,16,30,16,16,31}, {31,16,16,30,16,16,16},
        {14,17,16,23,17,17,15}, {17,17,17,31,17,17,17},
        {14,4,4,4,4,4,14}, {7,2,2,2,18,18,12},
        {17,18,20,24,20,18,17}, {16,16,16,16,16,16,31},
        {17,27,21,21,17,17,17}, {17,25,21,19,17,17,17},
        {14,17,17,17,17,17,14}, {30,17,17,30,16,16,16},
        {14,17,17,17,21,18,13}, {30,17,17,30,20,18,17},
        {15,16,16,14,1,1,30}, {31,4,4,4,4,4,4},
        {17,17,17,17,17,17,14}, {17,17,17,17,17,10,4},
        {17,17,17,21,21,21,10}, {17,17,10,4,10,17,17},
        {17,17,10,4,4,4,4}, {31,1,2,4,8,16,31}
    };
    static const unsigned char digits[10][7] = {
        {14,17,19,21,25,17,14}, {4,12,4,4,4,4,14},
        {14,17,1,2,4,8,31}, {30,1,1,14,1,1,30},
        {2,6,10,18,31,2,2}, {31,16,16,30,1,1,30},
        {14,16,16,30,17,17,14}, {31,1,2,4,8,8,8},
        {14,17,17,14,17,17,14}, {14,17,17,15,1,1,14}
    };
    GLboolean wasEnabled = glIsEnabled(GL_SCISSOR_TEST);
    GLint oldScissor[4];
    GLfloat oldColor[4];
    glGetIntegerv(GL_SCISSOR_BOX, oldScissor);
    glGetFloatv(GL_COLOR_CLEAR_VALUE, oldColor);
    glEnable(GL_SCISSOR_TEST);
    for (const char* letter = text; *letter; ++letter) {
        char c = *letter;
        if (c >= 'a' && c <= 'z') c -= 'a' - 'A';
        const unsigned char* glyph = nullptr;
        if (c >= 'A' && c <= 'Z') glyph = letters[c - 'A'];
        if (c >= '0' && c <= '9') glyph = digits[c - '0'];
        if (glyph) {
            for (int row = 0; row < 7; ++row) {
                for (int col = 0; col < 5; ++col) {
                    if (glyph[row] & (1 << (4 - col)))
                        drawRect(x + col * scale, y + (6 - row) * scale, scale, scale, .9f, 1, 1);
                }
            }
        } else if (c == ':') {
            drawRect(x + scale, y + 2 * scale, scale, scale, .9f, 1, 1);
            drawRect(x + scale, y + 4 * scale, scale, scale, .9f, 1, 1);
        } else if (c == '-') {
            drawRect(x, y + 3 * scale, 4 * scale, scale, .9f, 1, 1);
        }
        x += 6 * scale;
    }
    glScissor(oldScissor[0], oldScissor[1], oldScissor[2], oldScissor[3]);
    glClearColor(oldColor[0], oldColor[1], oldColor[2], oldColor[3]);
    if (!wasEnabled) glDisable(GL_SCISSOR_TEST);
}
