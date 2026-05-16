#include "GameView.h"
#include "core/GameEntity.h"
#include "core/GameConfig.h"
#include <QMouseEvent>
#include <QPainter>

GameView::GameView(QWidget *parent)
    : QGraphicsView(parent)
    , scene(nullptr)
{
    setRenderHint(QPainter::Antialiasing);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFixedSize(GameConfig::SceneWidth, GameConfig::SceneHeight);
}

void GameView::initScene()
{
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, GameConfig::SceneWidth, GameConfig::SceneHeight);
    setScene(scene);
    drawBackground();
    drawGrid();
}

void GameView::drawBackground()
{
    scene->setBackgroundBrush(QColor("#4a7c2e"));
}

void GameView::drawGrid()
{
    QPen pen(QColor("#3d6b24"), 1, Qt::SolidLine);
    for (int row = 0; row < GameConfig::Rows; ++row) {
        for (int col = 0; col < GameConfig::Cols; ++col) {
            qreal x = GameConfig::GridStartX + col * GameConfig::CellWidth;
            qreal y = GameConfig::GridStartY + row * GameConfig::CellHeight;
            auto* cell = scene->addRect(x, y, GameConfig::CellWidth, GameConfig::CellHeight, pen);
            cell->setZValue(1);
        }
    }
}

void GameView::addEntityItem(GameEntity* entity)
{
    if (scene && entity) {
        scene->addItem(entity);
    }
}

void GameView::removeEntityItem(GameEntity* entity)
{
    if (scene && entity) {
        scene->removeItem(entity);
    }
}

QPoint GameView::scenePosToCell(QPointF scenePos) const
{
    int col = static_cast<int>((scenePos.x() - GameConfig::GridStartX) / GameConfig::CellWidth);
    int row = static_cast<int>((scenePos.y() - GameConfig::GridStartY) / GameConfig::CellHeight);
    if (row < 0 || row >= GameConfig::Rows || col < 0 || col >= GameConfig::Cols)
        return QPoint(-1, -1);
    return QPoint(row, col);
}

QPointF GameView::cellToScenePos(int row, int col) const
{
    qreal x = GameConfig::GridStartX + col * GameConfig::CellWidth + GameConfig::CellWidth / 2.0;
    qreal y = GameConfig::GridStartY + row * GameConfig::CellHeight + GameConfig::CellHeight / 2.0;
    return QPointF(x, y);
}

QGraphicsScene* GameView::getScene() const
{
    return scene;
}

void GameView::mousePressEvent(QMouseEvent* event)
{
    QPointF scenePos = mapToScene(event->pos());
    QPoint cell = scenePosToCell(scenePos);
    if (cell.x() >= 0 && cell.y() >= 0) {
        emit cellClicked(cell.x(), cell.y());
    }
    QGraphicsView::mousePressEvent(event);
}
