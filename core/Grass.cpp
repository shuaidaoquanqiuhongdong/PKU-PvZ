#include "Grass.h"
#include <iostream>

using namespace std;

Grass::Grass()
{
    for (int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < 9; ++j)
        {
            grass[i][j] = nullptr;
        }
    }
}

Grass::~Grass() {}

// 判定此格子是否能种植植物
bool Grass::canPlant(int row, int col)
{
    if (row < 0 || row > 4 || col < 0 || col > 8)
        return false;
    return grass[row][col] == nullptr;
}

// 种植植物
void Grass::addPlant(int row, int col, Plant *p)
{
    if (canPlant(row, col))
    {
        grass[row][col] = p;
        cout << "你在" << row << "行" << col << "列" << "种植了一棵植物\n";
    }
    else if (row < 0 || row > 4 || col < 0 || col > 8)
        cout << "请在正确的位置种植植物！\n";
    else
        cout << "此处已有植物！\n";
}

// 铲除植物
void Grass::removePlant(int row, int col)
{
    if (grass[row][col] != nullptr)
    {
        delete grass[row][col];
        grass[row][col] = nullptr;
    }
}

// 返回此处的植物指针
Plant* Grass::getPlantAt(int row, int col)
{
    return grass[row][col];
}