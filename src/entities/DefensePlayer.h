#define DEFENSE_PLAYER_H

#include "Player.h"


class DefensePlayer : public Player {
private:
     bool  protectionEnabled;

public:
    DefensePlayer(float x, float y, float width, float height, int laneIndex, int maxHealth, float moveSpeed);
 virtual ~DefensePlayer() override;

    void update(float deltaTime) override;
    virtual PlayerType getPlayerType() const  override;
    virtual void startAttack() override;
    virtual bool canAttack() const override;
    virtual void jump() override;
    virtual void duck() override;

    bool canProvideProtection()  const;
    void disableProtection();
    bool protects(const  Player&  player) const;
};

