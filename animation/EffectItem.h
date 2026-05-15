#ifndef EFFECTITEM_H
#define EFFECTITEM_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QVector>

#include "SpriteAnimation.h"

class EffectItem : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    EffectItem(const QVector<QPixmap>& frames, int interval = 80, QObject* parent = nullptr);
    void play();

signals:
    void finished();

private:
    SpriteAnimation* animation;
};

#endif // EFFECTITEM_H
