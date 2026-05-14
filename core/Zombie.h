#pragma once
#include "GameEntity.h"

class Zombie : public GameEntity
{
protected:
    int speed; // 僵尸的移动速度
public:
    Zombie(int r, int c, int hp_, int speed_);
    virtual ~Zombie();
};