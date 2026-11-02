#pragma once

// A short introduction using the game's existing characters and artwork.
class Cutscene {
    float elapsed = 0;
    bool playing = false;
public:
    void start();
    void skip();
    void update(float deltaTime);
    bool isPlaying() const { return playing; }
    float getTime() const { return elapsed; }
    float getPlayerX() const;
    float getEnemyX() const;
    const char* getDialogue() const;
};
