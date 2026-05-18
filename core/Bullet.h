#pragma once
#include "GameEntity.h"
class Zombie;

class Bullet : public GameEntity
{
    Q_OBJECT
private:
    int damage, speed;
public:
    Bullet(int r, int c, int damage_, int speed_);
    ~Bullet();
    void updateEntity() override;
    bool collideWithZombie(Zombie* zombie);
};