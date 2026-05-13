#pragma once
#include "Plant.h"
#include "GameConfig.h"
//#include <QPoint>
//#include <QPointF>

class GridManager
{
private:
    Plant *grass[GameConfig::Rows][GameConfig::Cols]; // 每格草坪的指针
public:
    GridManager();
    ~GridManager();
    bool isValidCell(int row, int col) const;
    bool isCellEmpty(int row, int col) const;
    bool placePlant(Plant* plant, int row, int col);
    void removePlant(int row, int col);
    Plant* getPlant(int row, int col) const;
    // 这两个是用来把 格子(row, col) 和 屏幕像素坐标(x, y) 互相转换的工具
    //QPointF cellToScenePos(int row, int col) const;
    //QPoint scenePosToCell(QPointF scenePos) const;
};