
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
    void drawDigit(int digit, int x, int y)
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
                    x + rects[i][0],
                    y + rects[i][1],
                    rects[i][2],
                    rects[i][3],
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
    const int x = framebufferWidth - 210;
    const int y = framebufferHeight - 70;

    // Dark background
    drawRect(x, y, 198, 58,
             0.07f, 0.05f, 0.16f);

    // Gold coin icon
    drawRect(x + 20, y + 15, 20, 30,
             0.95f, 0.65f, 0.12f);

    drawRect(x + 15, y + 20, 30, 20,
             0.95f, 0.65f, 0.12f);

    drawRect(x + 27, y + 22, 4, 16,
             0.60f, 0.35f, 0.06f);

    // Current balance
    std::string balance =
        std::to_string(std::max(0, coins));

    int digitX = x + 64;

    for (char character : balance)
    {
        drawDigit(character - '0',
                  digitX, y + 14);

        digitX += 22;
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
