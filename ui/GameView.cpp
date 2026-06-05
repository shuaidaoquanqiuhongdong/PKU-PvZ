#include "GameView.h"
#include "core/GameEntity.h"
#include "core/GameConfig.h"
#include "GraphicsButton.h"
#include <QMouseEvent>
#include <QPainter>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QDebug>

GameView::GameView(QWidget *parent)
    : QGraphicsView(parent)
    , scene(nullptr)
    , pauseOverlay(nullptr)
    , hoverHighlight(nullptr)
{
    setRenderHint(QPainter::Antialiasing);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFixedSize(GameConfig::SceneWidth, GameConfig::SceneHeight);
    setMouseTracking(true);
    lastHoverCell = QPoint(-1, -1);
}

void GameView::applyScale(qreal scale)
{
    if (qFuzzyCompare(scale, 1.0))
        return;

    int w = static_cast<int>(GameConfig::SceneWidth * scale);
    int h = static_cast<int>(GameConfig::SceneHeight * scale);
    setFixedSize(w, h);
    setTransform(QTransform::fromScale(scale, scale));
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

    QString imagePath = QString(PROJECT_SOURCE_DIR) + "/resources/images/other/lawn.png";
    QPixmap bg(imagePath);
    if (!bg.isNull()) {
        auto* bgItem = scene->addPixmap(bg);
        bgItem->setPos(0, 0);
        bgItem->setZValue(0);
    }
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

    if (zombieMode) {
        QPen rightPen(QColor("#ff6b6b"), 2, Qt::DashLine);
        for (int row = 0; row < GameConfig::Rows; ++row) {
            for (int col = 5; col < GameConfig::Cols; ++col) {
                qreal x = GameConfig::GridStartX + col * GameConfig::CellWidth;
                qreal y = GameConfig::GridStartY + row * GameConfig::CellHeight;
                auto* cell = scene->addRect(x, y, GameConfig::CellWidth, GameConfig::CellHeight, rightPen);
                QBrush brush(QColor(255, 107, 107, 30));
                cell->setBrush(brush);
                cell->setZValue(2);
            }
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

void GameView::setPlantSelectionMode(bool enabled)
{
    plantSelectionEnabled = enabled;
    if (!enabled && hoverHighlight) {
        hoverHighlight->setVisible(false);
    }
}

void GameView::setZombieMode(bool enabled)
{
    zombieMode = enabled;
    if (scene) {
        drawGrid();
    }
}

void GameView::setShovelMode(bool enabled)
{
    shovelMode = enabled;
    if (!enabled && hoverHighlight) {
        hoverHighlight->setVisible(false);
    }
}

bool GameView::isRightSideColumn(int col) const
{
    return col >= 5 && col < GameConfig::Cols;
}

void GameView::mousePressEvent(QMouseEvent* event)
{
    if (pauseOverlay && pauseOverlay->isVisible()) {
        QGraphicsView::mousePressEvent(event);
        return;
    }

    QPointF scenePos = mapToScene(event->pos());
    QPoint cell = scenePosToCell(scenePos);
    if (cell.x() >= 0 && cell.y() >= 0) {
        if (shovelMode) {
            emit shovelCellClicked(cell.x(), cell.y());
        } else if (zombieMode) {
            if (isRightSideColumn(cell.y())) {
                emit zombieCellClicked(cell.x(), cell.y());
            }
        } else {
            emit cellClicked(cell.x(), cell.y());
        }
    }
    QGraphicsView::mousePressEvent(event);
}

void GameView::mouseMoveEvent(QMouseEvent* event)
{
    QPointF scenePos = mapToScene(event->pos());
    QPoint cell = scenePosToCell(scenePos);

    bool canHighlight = false;
    if (shovelMode) {
        canHighlight = cell.x() >= 0;
    } else if (zombieMode) {
        canHighlight = plantSelectionEnabled && cell.x() >= 0 && isRightSideColumn(cell.y());
    } else {
        canHighlight = plantSelectionEnabled && cell.x() >= 0;
    }

    if (canHighlight) {
        if (cell != lastHoverCell) {
            updateHoverHighlight(cell);
            lastHoverCell = cell;
        }
    } else {
        if (lastHoverCell.x() >= 0) {
            if (hoverHighlight) {
                hoverHighlight->setVisible(false);
            }
            lastHoverCell = QPoint(-1, -1);
        }
    }

    QGraphicsView::mouseMoveEvent(event);
}

void GameView::leaveEvent(QEvent* event)
{
    if (hoverHighlight) {
        hoverHighlight->setVisible(false);
    }
    lastHoverCell = QPoint(-1, -1);
    QGraphicsView::leaveEvent(event);
}

void GameView::updateHoverHighlight(const QPoint& cell)
{
    if (!scene) return;

    qreal x = GameConfig::GridStartX + cell.y() * GameConfig::CellWidth;
    qreal y = GameConfig::GridStartY + cell.x() * GameConfig::CellHeight;

    if (!hoverHighlight) {
        hoverHighlight = scene->addRect(x, y,
                                         GameConfig::CellWidth,
                                         GameConfig::CellHeight);
        hoverHighlight->setZValue(100);
    }

    if (shovelMode) {
        QColor highlightColor(255, 50, 50, 100);
        QPen pen(highlightColor, 3);
        hoverHighlight->setPen(pen);
        hoverHighlight->setBrush(QBrush(highlightColor, Qt::Dense5Pattern));
    } else if (zombieMode) {
        QColor highlightColor(255, 107, 107, 80);
        QPen pen(highlightColor, 3);
        hoverHighlight->setPen(pen);
        hoverHighlight->setBrush(QBrush(highlightColor, Qt::Dense4Pattern));
    } else {
        QColor highlightColor(255, 255, 100, 80);
        QPen pen(highlightColor, 3);
        hoverHighlight->setPen(pen);
        hoverHighlight->setBrush(QBrush(highlightColor, Qt::Dense4Pattern));
    }

    hoverHighlight->setRect(x, y, GameConfig::CellWidth, GameConfig::CellHeight);
    hoverHighlight->setVisible(true);
}

void GameView::setPaused(bool paused)
{
    if (!scene) return;

    if (!pauseOverlay) {
        pauseOverlay = new QGraphicsRectItem(0, 0,
            GameConfig::SceneWidth, GameConfig::SceneHeight);
        pauseOverlay->setBrush(QColor(0, 0, 0, 140));
        pauseOverlay->setPen(Qt::NoPen);
        pauseOverlay->setZValue(999);

        auto* textItem = new QGraphicsTextItem("暂停中", pauseOverlay);
        QFont font;
        font.setPointSize(48);
        font.setBold(true);
        textItem->setFont(font);
        textItem->setDefaultTextColor(Qt::white);
        textItem->setPos(
            (GameConfig::SceneWidth - textItem->boundingRect().width()) / 2.0,
            GameConfig::SceneHeight / 2.0 - 100
        );

        auto* backButton = new GraphicsButton("返回主菜单", pauseOverlay);
        backButton->setSize(200, 50);
        backButton->setNormalColor(QColor(76, 175, 80));
        backButton->setHoverColor(QColor(102, 187, 106));
        backButton->setPos(
            (GameConfig::SceneWidth - 200) / 2.0,
            GameConfig::SceneHeight / 2.0 + 20
        );
        connect(backButton, &GraphicsButton::clicked, this, [this]() {
            emit returnToMenuClicked();
        });

        scene->addItem(pauseOverlay);
    }

    pauseOverlay->setVisible(paused);
}
