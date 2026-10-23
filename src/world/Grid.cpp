#include "Grid.h"

Grid::Grid() {

    // Lane 0 - Top
    lanes[0] = {0, 0.21f, 0.98f, 0.40f, 0.51f, false, false};

    // Lane 1 - Upper Middle
    lanes[1] = {1, 0.21f, 0.98f, 0.51f, 0.63f, false, false};

    // Lane 2 - Lower Middle
    lanes[2] = {2, 0.21f, 0.98f, 0.63f, 0.78f, false, false};

    // Lane 3 - Bottom
    lanes[3] = {3, 0.21f, 0.98f, 0.78f, 0.93f, false, false};
}

const std::array<Lane, 4>& Grid::getLanes() const {
    return lanes;
}

int Grid::getLaneAtPosition(float x, float y) const {

    for (const Lane& lane : lanes) {

        if (x >= lane.left &&
            x <= lane.right &&
            y >= lane.top &&
            y < lane.bottom) {

            return lane.id;
        }
    }

    return -1;
}
void Grid::toggleLaneSelection(int laneId) {

    if (laneId < 0 || laneId >= 4) {
        return;
    }

    lanes[laneId].selected = !lanes[laneId].selected;
}