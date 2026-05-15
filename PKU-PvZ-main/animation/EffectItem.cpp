#include "EffectItem.h"

EffectItem::EffectItem(const QVector<QPixmap>& frames, int interval, QObject* parent)
    : QObject(parent)
    , QGraphicsPixmapItem()
    , animation(new SpriteAnimation(this, this))
{
    animation->setFrames(frames);
    animation->setFrameInterval(interval);
    animation->setLoop(false);

    if (!frames.isEmpty()) {
        setPixmap(frames.first());
        setOffset(-frames.first().width() / 2.0, -frames.first().height() / 2.0);
    }

    connect(animation, &SpriteAnimation::finished, this, &EffectItem::finished);
}

void EffectItem::play()
{
    animation->start();
}
