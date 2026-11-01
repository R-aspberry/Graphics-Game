
#include "HomeBase.h"

HomeBase::HomeBase()
{
    destroyed = false;
}

void HomeBase::enemyReached()
{
    destroyed = true;
}

bool HomeBase::isDestroyed() const
{
    return destroyed;
}

void HomeBase::reset()
{
    destroyed = false;
}
