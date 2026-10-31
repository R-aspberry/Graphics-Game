#include "Grid.h"


Grid::Grid() {
    lanes = {{
        // id, left, right, top, bottom, selected, hovered

        {0, 0.24f, 0.94f, 0.47f, 0.56f, false, false},

        {1, 0.23f, 0.95f, 0.56f, 0.67f, false, false},

        {2, 0.22f, 0.97f, 0.67f, 0.80f, false, false},

        {3, 0.21f, 0.98f, 0.80f, 0.92f, false, false}
    }};
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