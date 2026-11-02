
#pragma once

class HUD
{
public:
    bool init()
    {
        return true;
    }

    void drawCoins(int coins,int framebufferWidth,int framebufferHeight);

    void drawhealth(int x,int y,int w,int h,int health,int maxHealth);
    void drawText(const char* text, int x, int y, int scale = 2);


    void cleanup()
    {
    }
};
