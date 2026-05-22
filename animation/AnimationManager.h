#ifndef ANIMATIONMANAGER_H
#define ANIMATIONMANAGER_H

#include <QObject>
#include <QHash>
#include <QPointF>
#include <QPixmap>
#include <QVector>

class QAbstractAnimation;
class QGraphicsScene;

class GameEntity;
class Plant;
class Zombie;
class Bullet;
class Sun;
class EffectItem;
class SpriteAnimation;

enum class AnimationState {
    Idle,
    Walk,
    Attack,
    Hit,
    Death,
    Produce,
    Spawn,
    Collect,
    Move
};

enum ZLayer {
    BackgroundLayer = 0,
    GridLayer = 1,
    PlantLayer = 10,
    ZombieLayer = 20,
    BulletLayer = 30,
    SunLayer = 40,
    EffectLayer = 50,
    UILayer = 100
};

inline size_t qHash(AnimationState state, size_t seed = 0) noexcept
{
    return ::qHash(static_cast<int>(state), seed);
}

class AnimationManager : public QObject {
    Q_OBJECT

public:
    explicit AnimationManager(QObject* parent = nullptr);

    void bindEntity(GameEntity* entity);
    void unbindEntity(GameEntity* entity);
    void playAnimation(GameEntity* entity, AnimationState state);

    void playHitEffect(GameEntity* entity);
    void playDeathEffect(GameEntity* entity);
    void playPlantPlaceEffect(Plant* plant);

    void playBulletHitEffect(QPointF pos);

    // Prompt 标准接口：需要调用者给出起点和终点。
    void playSunSpawnAnimation(Sun* sun, QPointF start, QPointF end);
    void playSunIdleAnimation(Sun* sun);
    void playSunCollectAnimation(Sun* sun, QPointF targetPos);

    // 仓库现有 GameEngine 兼容接口：
    // GameEngine::sunCreated(Sun*) 可以直接连接到这里。
    void playSunSpawnAnimation(Sun* sun);

    // GameEngine::sunCollected(Sun*) 可以直接连接到这里。
    void playSunCollectAnimation(Sun* sun);

signals:
    void deathAnimationFinished(GameEntity* entity);
    void sunCollectAnimationFinished(Sun* sun);
    void effectFinished(EffectItem* effect);

private:
    QHash<GameEntity*, QHash<AnimationState, SpriteAnimation*>> animations;
    QHash<GameEntity*, AnimationState> currentStates;

    // 用于播放 bullet hit effect，因为 playBulletHitEffect 只有 QPointF 参数，没有 scene 参数。
    QGraphicsScene* lastKnownScene = nullptr;

    // 阳光的 Spawn / Idle / Collect 位置动画。
    QHash<Sun*, QAbstractAnimation*> sunMotionAnimations;

    void setupPlantAnimations(Plant* plant);
    void setupZombieAnimations(Zombie* zombie);
    void setupBulletAnimations(Bullet* bullet);
    void setupSunAnimations(Sun* sun);

    int animationPriority(AnimationState state) const;

    void addSpriteAnimation(
        GameEntity* entity,
        AnimationState state,
        const QVector<QPixmap>& frames,
        int interval,
        bool loop
    );

    void stopSunMotionAnimation(Sun* sun);
    QPointF defaultSunCollectTarget() const;
};

#endif // ANIMATIONMANAGER_H
