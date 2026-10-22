#pragma once

#include "Lane.h"
#include <array>

class Grid {
public:
    Grid();

    const std::array<Lane, 4>& getLanes() const;

    int getLaneAtPosition(float x, float y) const;
    void toggleLaneSelection(int laneId);

private:
    std::array<Lane, 4> lanes;
};