#pragma once
#include "GameEntity.h"
#include <QString>

class Plant : public GameEntity
{
protected:
    int cost; // 植物消耗的阳光数量
    QString plantType;
public:
    Plant(int r, int c, int hp_, int cost_, QString planttype);
    ~Plant();
    void updateEntity() override;
};