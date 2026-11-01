
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


    void cleanup()
    {
    }
};
