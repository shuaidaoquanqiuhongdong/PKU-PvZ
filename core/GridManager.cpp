#include "GridManager.h"
#include <iostream>

using namespace std;
using namespace GameConfig;

GridManager::GridManager()
{
    for (int i = 0; i < Rows; ++i)
    {
        for (int j = 0; j < Cols; ++j)
        {
            grid[i][j] = nullptr;
        }
    }
}

GridManager::~GridManager() {}

// 判定此格子位置是否合法
bool GridManager::isValidCell(int row, int col) const
{
    return row >= 0 && row < Rows && col >= 0 && col < Cols;
}

// 判定此格子是否为空格子
bool GridManager::isCellEmpty(int row, int col) const
{
    if (!isValidCell(row, col))
    {
        return false;
    }
    return grid[row][col] == nullptr;
}

// 种植植物
bool GridManager::placePlant(Plant* plant, int row, int col)
{
    if (isCellEmpty(row, col))
    {
        grid[row][col] = plant;
        cout << "你在" << row + 1 << "行" << col + 1 << "列" << "种植了一棵植物\n";
        return true;
    }
    else
    {
        cout << "请在正确的位置种植植物！\n";
        return false;
    }
}

// 铲除植物
void GridManager::removePlant(int row, int col)
{
    if (!isValidCell(row, col))
    {
        return;
    }
    grid[row][col] = nullptr;

}

// 返回此处的植物指针
Plant* GridManager::getPlant(int row, int col) const
{
    if (!isValidCell(row, col))
    {
        return nullptr;
    }
    return grid[row][col];
}

QPointF GridManager::cellToScenePos(int row, int col) const
{
    qreal x = GameConfig::GridStartX + col * GameConfig::CellWidth + GameConfig::CellWidth / 2.0;
    qreal y = GameConfig::GridStartY + row * GameConfig::CellHeight + GameConfig::CellHeight / 2.0;
    return QPointF(x, y);
}

QPoint GridManager::scenePosToCell(QPointF scenePos) const
{
    int col = (scenePos.x() - GameConfig::GridStartX) / GameConfig::CellWidth;
    int row = (scenePos.y() - GameConfig::GridStartY) / GameConfig::CellHeight;
    // 判断一下玩家是不是点到草坪外面去了
    if (row < 0 || row >= GameConfig::Rows || col < 0 || col >= GameConfig::Cols)
    {
        return QPoint(-1, -1); // 用 -1 代表无效点击
    }
    return QPoint(col, row); // 返回列和行
}