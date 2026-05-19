#pragma once
#include "GameEntity.h"

class Sun : public GameEntity
{
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
private:
    int value;
    bool collected;
public:
    Sun(int value_);
    ~Sun();
    int getValue() const;
    void collect();
signals:
    void clicked(Sun* sun);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
};
