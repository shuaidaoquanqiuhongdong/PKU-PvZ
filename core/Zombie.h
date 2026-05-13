#pragma once
#include "Entity.h"

class Zombie : public Entity
{
protected:
    int speed; // 僵尸的移动速度
public:
    Zombie(int r, int c, int hp_, int speed_);
    virtual ~Zombie();
};