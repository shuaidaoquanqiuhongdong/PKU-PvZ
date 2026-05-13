#pragma once
#include "Entity.h"

class Plant : public Entity
{
protected:
    int cost; // 植物消耗的阳光数量
public:
    Plant(int r, int c, int hp_, int cost_);
    virtual ~Plant();
};