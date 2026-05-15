#pragma once
#include "Plant.h"
#include "GameConfig.h"
#include <QPoint>
#include <QPointF>

class GridManager
{
private:
    Plant *grid[GameConfig::Rows][GameConfig::Cols]; // 每格草坪的指针
public:
    GridManager();
    ~GridManager();
    bool isValidCell(int row, int col) const;
    bool isCellEmpty(int row, int col) const;
    bool placePlant(Plant* plant, int row, int col);
    void removePlant(int row, int col);
    Plant* getPlant(int row, int col) const;
    QPointF cellToScenePos(int row, int col) const;
    QPoint scenePosToCell(QPointF scenePos) const;
};