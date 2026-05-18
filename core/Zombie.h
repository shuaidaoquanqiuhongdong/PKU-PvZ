#pragma once
#include "GameEntity.h"

class Zombie : public GameEntity
{
protected:
    int speed; // 僵尸的移动速度
public:
    Zombie(int r, int c, int hp_, int speed_);
    ~Zombie();
    void updateEntity() override;
    void moveLeft();
    int getRow();
};

class GenziZombie : public Zombie
{
public:
    GenziZombie(int row_, int col_);
    ~GenziZombie();
};