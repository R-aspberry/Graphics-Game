
#pragma once

class HUD
{
public:
    bool init()
    {
        return true;
    }

    void drawCoins(
        int coins,
        int framebufferWidth,
        int framebufferHeight
    );

    void cleanup()
    {
    }
};
