#ifndef ANIMATIONMANAGER_H
#define ANIMATIONMANAGER_H

#include <QObject>
#include <QHash>
#include <QPointF>

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
    void playSunSpawnAnimation(Sun* sun, QPointF start, QPointF end);
    void playSunIdleAnimation(Sun* sun);
    void playSunCollectAnimation(Sun* sun, QPointF targetPos);

signals:
    void deathAnimationFinished(GameEntity* entity);
    void sunCollectAnimationFinished(Sun* sun);
    void effectFinished(EffectItem* effect);

private:
    QHash<GameEntity*, QHash<AnimationState, SpriteAnimation*>> animations;
    QHash<GameEntity*, AnimationState> currentStates;

    void setupPlantAnimations(Plant* plant);
    void setupZombieAnimations(Zombie* zombie);
    void setupBulletAnimations(Bullet* bullet);
    void setupSunAnimations(Sun* sun);

    int animationPriority(AnimationState state) const;
};

#endif // ANIMATIONMANAGER_H
