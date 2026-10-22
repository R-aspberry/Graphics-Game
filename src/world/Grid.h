#pragma once

#include "Lane.h"
#include <array>

class Grid {
public:
    Grid();

    const std::array<Lane, 3>& getLanes() const;

    int getLaneAtPosition(float x, float y) const;

private:
    std::array<Lane, 3> lanes;
};