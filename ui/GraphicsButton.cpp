#include "GraphicsButton.h"
#include <QGraphicsSceneHoverEvent>
#include <QPainter>

GraphicsButton::GraphicsButton(const QString& text, QGraphicsItem* parent)
    : QGraphicsRectItem(parent)
    , textItem(new QGraphicsTextItem(text, this))
    , normalColor(76, 175, 80)
    , hoverColor(102, 187, 106)
{
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIsFocusable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);

    QFont font;
    font.setPointSize(16);
    font.setBold(true);
    textItem->setFont(font);
    textItem->setDefaultTextColor(Qt::white);
    textItem->setPos(0, 0);

    setBrush(normalColor);
    setPen(QPen(Qt::white, 2));
}

void GraphicsButton::setSize(qreal width, qreal height)
{
    setRect(0, 0, width, height);
    
    QRectF textRect = textItem->boundingRect();
    textItem->setPos(
        (width - textRect.width()) / 2.0,
        (height - textRect.height()) / 2.0
    );
}

void GraphicsButton::setNormalColor(const QColor& color)
{
    normalColor = color;
    if (!pressed) {
        setBrush(normalColor);
    }
}

void GraphicsButton::setHoverColor(const QColor& color)
{
    hoverColor = color;
}

void GraphicsButton::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    Q_UNUSED(event);
    setBrush(hoverColor);
}

void GraphicsButton::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    Q_UNUSED(event);
    setBrush(normalColor);
}

void GraphicsButton::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    Q_UNUSED(event);
    pressed = true;
    QColor darkerColor = normalColor.darker(120);
    setBrush(darkerColor);
}

void GraphicsButton::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    Q_UNUSED(event);
    if (pressed) {
        pressed = false;
        setBrush(normalColor);
        emit clicked();
    }
}
