#pragma once
#include "Plant.h"

class Grass
{
private:
    Plant *grass[5][9]; // 每格草坪的指针
public:
    Grass();
    ~Grass();
    // 判定此格子是否能种植植物
    bool canPlant(int row, int col);
    void addPlant(int row, int col, Plant *p);
    // 铲除植物
    void removePlant(int row, int col);
    // 返回此处的植物指针
    Plant *getPlantAt(int row, int col);
};