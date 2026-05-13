#pragma once
#include "Grass.h"
#include "Zombie.h"
#include <vector>

class GameManager
{
private:
    Grass grass;
    std::vector<Zombie *> zombies; // 存放当前场上的所有僵尸
    int sunlight;             // 玩家当前的阳光储备
    int frameCount;           // 游戏当前进行到多少帧
public:
    GameManager(); // 开局50阳光
    void addZombie(Zombie *z);
    // 提供给前端的种植物接口
    void buyAndPlant(int row, int col, Plant *p);
    // 游戏主循环的一帧
    void update();
};