#ifndef DEFENSE_PLAYER_H
#define DEFENSE_PLAYER_H

#include "Player.h"


class DefensePlayer : public Player {

private:
     bool  protectionEnabled;

public:
    DefensePlayer(float x, float y, float width, float height, int laneIndex, int maxHealth, float moveSpeed);
    virtual ~DefensePlayer() override;

    void update(float deltaTime) override;
    PlayerType getPlayerType() const  override;
    void startAttack(GameManager &gameManager ) override;
    bool canAttack() const override;
    void jump() override;
    void duck() override;

    bool canProvideProtection()  const;
    void disableProtection();
    bool protects(const  Player&  player) const;
};

#endif