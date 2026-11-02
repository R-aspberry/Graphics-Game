#ifndef ANIMATION_H
#define ANIMATION_H
#include <cmath>

class Animation {
    float elapsed = 0.0f;
public:
    void update(float deltaTime) {
        if (std::isfinite(deltaTime) && deltaTime > 0) elapsed = std::fmod(elapsed + deltaTime, 60.0f);
    }
    int getFrame(int frameCount = 4, float framesPerSecond = 8.0f) const {
        if (frameCount <= 0 || framesPerSecond <= 0) return 0;
        return static_cast<int>(elapsed * framesPerSecond) % frameCount;
    }
    void reset() { elapsed = 0; }
};
#endif
