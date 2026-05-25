#include "Sun.h"
#include <QGraphicsSceneMouseEvent>

Sun::Sun(int value_) : GameEntity(0, 0, 1, EntityType::Sun), value(value_), collected(false)
{
    setAcceptedMouseButtons(Qt::LeftButton);
}

Sun::~Sun() {}

int Sun::getValue() const { return value; }

void Sun::collect()
{
    if (!collected)
    {
        collected = true;
    }
}

void Sun::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    Q_UNUSED(event);
    if (!collected)
        emit clicked(this);
}