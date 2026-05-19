#include "SpriteAnimation.h"

SpriteAnimation::SpriteAnimation(QGraphicsPixmapItem* target, QObject* parent)
    : QObject(parent)
    , target(target)
    , timer(new QTimer(this))
    , currentFrame(0)
    , frameInterval(160)
    , loop(true)
    , running(false)
{
    connect(timer, &QTimer::timeout, this, &SpriteAnimation::nextFrame);
}

void SpriteAnimation::setFrames(const QVector<QPixmap>& frames)
{
    this->frames = frames;

    if (target && !this->frames.isEmpty()) {
        target->setPixmap(this->frames.first());
    }
}

void SpriteAnimation::setFrameInterval(int ms)
{
    frameInterval = ms;
    timer->setInterval(frameInterval);
}

void SpriteAnimation::setLoop(bool loop)
{
    this->loop = loop;
}

void SpriteAnimation::start()
{
    if (!target || frames.isEmpty()) {
        return;
    }

    running = true;
    currentFrame = 0;
    target->setPixmap(frames[currentFrame]);

    timer->start(frameInterval);
}

void SpriteAnimation::stop()
{
    timer->stop();
    running = false;
}

void SpriteAnimation::reset()
{
    currentFrame = 0;

    if (target && !frames.isEmpty()) {
        target->setPixmap(frames[currentFrame]);
    }
}

bool SpriteAnimation::isRunning() const
{
    return running;
}

void SpriteAnimation::nextFrame()
{
    if (!target || frames.isEmpty()) {
        stop();
        return;
    }

    ++currentFrame;

    if (currentFrame >= frames.size()) {
        if (loop) {
            currentFrame = 0;
        }
        else {
            stop();
            emit finished();
            return;
        }
    }

    target->setPixmap(frames[currentFrame]);
}
