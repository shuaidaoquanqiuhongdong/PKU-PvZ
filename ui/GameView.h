#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QPoint>

class GameEntity;

class GameView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GameView(QWidget *parent = nullptr);
    void applyScale(qreal scale);
    void initScene();
    void drawBackground();
    void drawGrid();
    void addEntityItem(GameEntity* entity);
    void removeEntityItem(GameEntity* entity);
    QPoint scenePosToCell(QPointF scenePos) const;
    QPointF cellToScenePos(int row, int col) const;
    QGraphicsScene* getScene() const;
    void setPaused(bool paused);
    void setPlantSelectionMode(bool enabled);
    void setZombieMode(bool enabled);
    void setShovelMode(bool enabled);
signals:
    void cellClicked(int row, int col);
    void zombieCellClicked(int row, int col);
    void shovelCellClicked(int row, int col);
    void returnToMenuClicked();
protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;
private:
    QGraphicsScene* scene;
    QGraphicsRectItem* pauseOverlay = nullptr;
    QGraphicsRectItem* hoverHighlight = nullptr;
    QPoint lastHoverCell;
    bool plantSelectionEnabled = false;
    bool zombieMode = false;
    bool shovelMode = false;
    void updateHoverHighlight(const QPoint& cell);
    bool isRightSideColumn(int col) const;
};

#endif
