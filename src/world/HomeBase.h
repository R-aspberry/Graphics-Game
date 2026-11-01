
#pragma once

class HomeBase
{
private:
    bool destroyed;

public:
    HomeBase();

    void enemyReached();
    bool isDestroyed() const;
    void reset();
};
