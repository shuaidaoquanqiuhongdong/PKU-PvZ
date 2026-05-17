#include "AnimationManager.h"

#include "ResourceManager.h"
#include "SpriteAnimation.h"
#include "EffectItem.h"
#include "core/GameEntity.h"
#include "core/Plant.h"
#include "core/Zombie.h"
//#include "core/Bullet.h"
//#include "core/Sun.h"

#include <QGraphicsScene>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QEasingCurve>
#include <QDebug>
#include <QSequentialAnimationGroup>

AnimationManager::AnimationManager(QObject* parent)
    : QObject(parent)
{
}

void AnimationManager::bindEntity(GameEntity* entity)
{
    if (!entity) {
        return;
    }

    if (animations.contains(entity)) {
        return;
    }

    animations.insert(entity, QHash<AnimationState, SpriteAnimation*>());

    switch (entity->getEntityType()) {
    case EntityType::Plant:
        setupPlantAnimations(static_cast<Plant*>(entity));
        break;

    case EntityType::Zombie:
        setupZombieAnimations(static_cast<Zombie*>(entity));
        break;

    case EntityType::Bullet:
    case EntityType::Sun:

    case EntityType::Effect:
        break;
    }
}

void AnimationManager::unbindEntity(GameEntity* entity)
{
    if (!entity) {
        return;
    }

    if (!animations.contains(entity)) {
        return;
    }

    auto entityAnimations = animations.take(entity);

    for (SpriteAnimation* animation : entityAnimations) {
        if (animation) {
            animation->stop();
            animation->deleteLater();
        }
    }

    currentStates.remove(entity);
}

void AnimationManager::playAnimation(GameEntity* entity, AnimationState state)
{
    if (!entity) {
        return;
    }

    if (!animations.contains(entity)) {
        bindEntity(entity);
    }

    const AnimationState oldState = currentStates.value(entity, AnimationState::Idle);

    if (oldState == AnimationState::Death) {
        return;
    }

    if (animationPriority(state) < animationPriority(oldState)) {
        return;
    }

    if (state == AnimationState::Hit) {
        playHitEffect(entity);
        return;
    }

    if (state == AnimationState::Death) {
        playDeathEffect(entity);
        return;
    }

    auto& table = animations[entity];

    if (!table.contains(state)) {
        qWarning() << "AnimationManager: missing animation state:" << static_cast<int>(state);
        return;
    }

    if (table.contains(oldState) && table[oldState]) {
        table[oldState]->stop();
    }

    currentStates[entity] = state;

    SpriteAnimation* animation = table[state];
    if (animation) {
        animation->start();
    }
}

void AnimationManager::playHitEffect(GameEntity* entity)
{
    if (!entity) {
        return;
    }

    auto* effect = new QGraphicsOpacityEffect(entity);
    entity->setGraphicsEffect(effect);

    auto* fadeOut = new QPropertyAnimation(effect, "opacity", this);
    fadeOut->setDuration(100);
    fadeOut->setStartValue(1.0);
    fadeOut->setEndValue(0.35);

    auto* fadeIn = new QPropertyAnimation(effect, "opacity", this);
    fadeIn->setDuration(100);
    fadeIn->setStartValue(0.35);
    fadeIn->setEndValue(1.0);

    auto* group = new QSequentialAnimationGroup(this);
    group->addAnimation(fadeOut);
    group->addAnimation(fadeIn);

    connect(group, &QSequentialAnimationGroup::finished, this, [entity, effect, group]() {
        if (entity) {
            entity->setGraphicsEffect(nullptr);
        }

        effect->deleteLater();
        group->deleteLater();
        });

    group->start();
}

void AnimationManager::playDeathEffect(GameEntity* entity)
{
    if (!entity) {
        return;
    }

    currentStates[entity] = AnimationState::Death;

    if (animations.contains(entity)) {
        auto& table = animations[entity];

        for (SpriteAnimation* animation : table) {
            if (animation) {
                animation->stop();
            }
        }
    }

    auto* effect = new QGraphicsOpacityEffect(entity);
    entity->setGraphicsEffect(effect);

    auto* animation = new QPropertyAnimation(effect, "opacity", this);
    animation->setDuration(500);
    animation->setStartValue(1.0);
    animation->setEndValue(0.0);
    animation->setEasingCurve(QEasingCurve::OutQuad);

    connect(animation, &QPropertyAnimation::finished, this, [this, entity, effect, animation]() {
        if (entity) {
            entity->setGraphicsEffect(nullptr);
        }

        effect->deleteLater();
        animation->deleteLater();

        emit deathAnimationFinished(entity);
        });

    animation->start();
}

void AnimationManager::playPlantPlaceEffect(Plant* plant)
{
    if (!plant || !plant->scene()) {
        return;
    }

    QVector<QPixmap> frames =
        ResourceManager::loadFrames(":/images/effects", "place", 2);

    auto* effect = new EffectItem(frames, 80, this);
    effect->setPos(plant->pos());
    effect->setZValue(EffectLayer);

    plant->scene()->addItem(effect);

    connect(effect, &EffectItem::finished, this, [this, effect]() {
        if (effect->scene()) {
            effect->scene()->removeItem(effect);
        }

        emit effectFinished(effect);
        effect->deleteLater();
        });

    effect->play();
}

void AnimationManager::playBulletHitEffect(QPointF pos)
{
    Q_UNUSED(pos)

        // 当前阶段不实现子弹命中特效。
        // 后续需要时添加 resources/images/effects/pea_hit_0.png 等资源，
        // 并在这里创建 EffectItem。
}

void AnimationManager::playSunSpawnAnimation(Sun* sun, QPointF start, QPointF end)
{
    Q_UNUSED(sun)
    Q_UNUSED(start)
    Q_UNUSED(end)
    // 暂不实现阳光生成动画
}

void AnimationManager::playSunIdleAnimation(Sun* sun)
{
    Q_UNUSED(sun)
    // 暂不实现阳光空闲动画
}

void AnimationManager::playSunCollectAnimation(Sun* sun, QPointF targetPos)
{
    Q_UNUSED(sun)
    Q_UNUSED(targetPos)
    // 暂不实现阳光收集动画
}

void AnimationManager::setupPlantAnimations(Plant* plant)
{
    if (!plant) {
        return;
    }

    QVector<QPixmap> idleFrames =
        ResourceManager::loadFrames(":/images/plants/shooter", "idle", 3);

    addSpriteAnimation(
        plant,
        AnimationState::Idle,
        idleFrames,
        180,
        true
    );

    if (!idleFrames.isEmpty()) {
        plant->setPixmap(idleFrames.first());
        plant->setOffset(
            -idleFrames.first().width() / 2.0,
            -idleFrames.first().height() / 2.0
        );
    }

    plant->setZValue(PlantLayer);
}

void AnimationManager::setupZombieAnimations(Zombie* zombie)
{
    if (!zombie) {
        return;
    }

    QVector<QPixmap> walkFrames =
        ResourceManager::loadFrames(":/images/zombies/normal", "walk", 4);

    addSpriteAnimation(
        zombie,
        AnimationState::Walk,
        walkFrames,
        160,
        true
    );

    if (!walkFrames.isEmpty()) {
        zombie->setPixmap(walkFrames.first());
        zombie->setOffset(
            -walkFrames.first().width() / 2.0,
            -walkFrames.first().height() / 2.0
        );
    }

    zombie->setZValue(ZombieLayer);
}

void AnimationManager::setupBulletAnimations(Bullet* bullet)
{
    Q_UNUSED(bullet)

        // 当前阶段不实现子弹动画。
}

void AnimationManager::setupSunAnimations(Sun* sun)
{
    Q_UNUSED(sun)

        // 当前阶段不实现阳光动画绑定。
        // Sun 的 Spawn / Idle / Collect 动画已通过独立接口保留。
}

void AnimationManager::addSpriteAnimation(
    GameEntity* entity,
    AnimationState state,
    const QVector<QPixmap>& frames,
    int interval,
    bool loop
) {
    if (!entity) {
        return;
    }

    auto* animation = new SpriteAnimation(entity, this);
    animation->setFrames(frames);
    animation->setFrameInterval(interval);
    animation->setLoop(loop);

    animations[entity].insert(state, animation);

    if (!loop) {
        connect(animation, &SpriteAnimation::finished, this, [this, entity, state]() {
            if (!entity) {
                return;
            }

            if (state == AnimationState::Death) {
                emit deathAnimationFinished(entity);
                return;
            }

            currentStates[entity] = AnimationState::Idle;
            playAnimation(entity, AnimationState::Idle);
            });
    }
}

int AnimationManager::animationPriority(AnimationState state) const
{
    switch (state) {
    case AnimationState::Death:
        return 100;

    case AnimationState::Hit:
        return 80;

    case AnimationState::Attack:
    case AnimationState::Produce:
        return 60;

    case AnimationState::Walk:
        return 30;

    case AnimationState::Idle:
        return 10;

    case AnimationState::Spawn:
    case AnimationState::Collect:
    case AnimationState::Move:
        return 20;
    }

    return 0;
}
