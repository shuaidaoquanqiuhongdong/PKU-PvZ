#ifndef GRAPHICSBUTTON_H
#define GRAPHICSBUTTON_H

#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsSceneMouseEvent>

class GraphicsButton : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    explicit GraphicsButton(const QString& text, QGraphicsItem* parent = nullptr);
    void setSize(qreal width, qreal height);
    void setNormalColor(const QColor& color);
    void setHoverColor(const QColor& color);
signals:
    void clicked();
protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
private:
    QGraphicsTextItem* textItem;
    QColor normalColor;
    QColor hoverColor;
    bool pressed = false;
};

#endif
