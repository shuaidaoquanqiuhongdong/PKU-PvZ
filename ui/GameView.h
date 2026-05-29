#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
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
signals:
    void cellClicked(int row, int col);
protected:
    void mousePressEvent(QMouseEvent* event) override;
private:
    QGraphicsScene* scene;
};

#endif
