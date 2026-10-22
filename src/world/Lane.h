#pragma once

struct Lane {
    int id;

    float left;
    float right;
    float top;
    float bottom;

    bool selected = false;
    bool hovered = false;
};