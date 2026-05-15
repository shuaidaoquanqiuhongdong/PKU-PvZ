#ifndef SPRITEANIMATION_H
#define SPRITEANIMATION_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QVector>
#include <QTimer>

class SpriteAnimation : public QObject {
    Q_OBJECT

public:
    explicit SpriteAnimation(QGraphicsPixmapItem* target, QObject* parent = nullptr);

    void setFrames(const QVector<QPixmap>& frames);
    void setFrameInterval(int ms);
    void setLoop(bool loop);
    void start();
    void stop();
    void reset();
    bool isRunning() const;

signals:
    void finished();

private slots:
    void nextFrame();

private:
    QGraphicsPixmapItem* target;
    QVector<QPixmap> frames;
    QTimer* timer;
    int currentFrame;
    int frameInterval;
    bool loop;
    bool running;
};

#endif // SPRITEANIMATION_H
