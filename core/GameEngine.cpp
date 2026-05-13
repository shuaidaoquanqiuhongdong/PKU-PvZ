#include "GameEngine.h"
#include <iostream>

using namespace std;

GameManager::GameManager() : sunlight(50), frameCount(0) {}

void GameManager::addZombie(Zombie *z)
{
    zombies.push_back(z);
    cout << "生成了一只僵尸！" << endl;
}

// 提供给前端的种植物接口
void GameManager::buyAndPlant(int row, int col, Plant *p)
{
    grass.addPlant(row, col, p);
}

// 游戏主循环的一帧
void GameManager::update()
{
    ++frameCount;
    cout << "--- 当前游戏帧：" << frameCount << " ---" << endl;
    // 让草坪上所有的植物都行动一次
    for (int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < 9; ++j)
        {
            Plant *p = grass.getPlantAt(i, j);
            if (p != nullptr && !p->isDead())
            {
                p->tick();
            }
        }
    }
    // 让场上所有的僵尸都行动一次
    for (auto z : zombies)
    {
        if (!z->isDead())
        {
            z->tick();
        }
    }
    // 战场打扫 ：把死掉的僵尸清理掉
    for (auto it = zombies.begin(); it != zombies.end();)
    {
        if ((*it)->isDead())
        {
            cout << "清理了一具僵尸的尸体！" << endl;
            delete *it;             // 释放内存
            it = zombies.erase(it); // 从数组里剔除
        }
        else
        {
            ++it;
        }
    }
    // 打扫植物的尸体
    for (int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < 9; ++j)
        {
            Plant *p = grass.getPlantAt(i, j);
            if (p != nullptr && p->isDead())
            {
                cout << i << "行" << j << "列 的植物阵亡了！" << endl;
                grass.removePlant(i, j);
            }
        }
    }
}