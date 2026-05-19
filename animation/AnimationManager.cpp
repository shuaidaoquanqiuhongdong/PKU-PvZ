#include "AnimationManager.h"

#include "ResourceManager.h"
#include "SpriteAnimation.h"
#include "EffectItem.h"

#include "core/GameEntity.h"
#include "core/Plant.h"
#include "core/Zombie.h"
#include "core/Bullet.h"
#include "core/Sun.h"

#include <QGraphicsScene>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QAbstractAnimation>
#include <QEasingCurve>
#include <QPointer>
#include <QDebug>

AnimationManager::AnimationManager(QObject* parent)
    : QObject(parent)
{
}

void AnimationManager::bindEntity(GameEntity* entity)
{
    if (!entity) {
        return;
    }

    if (entity->scene()) {
        lastKnownScene = entity->scene();
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
        setupBulletAnimations(static_cast<Bullet*>(entity));
        break;

    case EntityType::Sun:
        setupSunAnimations(static_cast<Sun*>(entity));
        break;

    case EntityType::Effect:
        break;
    }

    connect(entity, &QObject::destroyed, this, [this, entity]() {
        currentStates.remove(entity);
        animations.remove(entity);
        });
}

void AnimationManager::unbindEntity(GameEntity* entity)
{
    if (!entity) {
        return;
    }

    if (entity->getEntityType() == EntityType::Sun) {
        stopSunMotionAnimation(static_cast<Sun*>(entity));
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
        // Bullet::Move 当前使用单张静态图，不一定需要 SpriteAnimation。
        if (entity->getEntityType() != EntityType::Bullet) {
            qWarning() << "AnimationManager: missing animation state:"
                << static_cast<int>(state);
        }
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

    auto* effect = new QGraphicsOpacityEffect();
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

    QPointer<GameEntity> safeEntity(entity);

    connect(group, &QSequentialAnimationGroup::finished, this, [safeEntity, effect, group]() {
        if (safeEntity) {
            safeEntity->setGraphicsEffect(nullptr);
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

    auto* effect = new QGraphicsOpacityEffect();
    entity->setGraphicsEffect(effect);

    auto* animation = new QPropertyAnimation(effect, "opacity", this);
    animation->setDuration(500);
    animation->setStartValue(1.0);
    animation->setEndValue(0.0);
    animation->setEasingCurve(QEasingCurve::OutQuad);

    QPointer<GameEntity> safeEntity(entity);

    connect(animation, &QPropertyAnimation::finished, this, [this, safeEntity, effect, animation]() {
        if (safeEntity) {
            safeEntity->setGraphicsEffect(nullptr);
            emit deathAnimationFinished(safeEntity.data());
        }

        effect->deleteLater();
        animation->deleteLater();
        });

    animation->start();
}

void AnimationManager::playPlantPlaceEffect(Plant* plant)
{
    if (!plant || !plant->scene()) {
        return;
    }

    lastKnownScene = plant->scene();

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
    if (!lastKnownScene) {
        return;
    }

    QVector<QPixmap> frames =
        ResourceManager::loadFrames(":/images/effects", "pea_hit", 3);

    auto* effect = new EffectItem(frames, 70, this);
    effect->setPos(pos);
    effect->setZValue(EffectLayer);

    lastKnownScene->addItem(effect);

    connect(effect, &EffectItem::finished, this, [this, effect]() {
        if (effect->scene()) {
            effect->scene()->removeItem(effect);
        }

        emit effectFinished(effect);
        effect->deleteLater();
        });

    effect->play();
}

void AnimationManager::playSunSpawnAnimation(Sun* sun, QPointF start, QPointF end)
{
    if (!sun) {
        return;
    }

    if (sun->scene()) {
        lastKnownScene = sun->scene();
    }

    stopSunMotionAnimation(sun);

    currentStates[sun] = AnimationState::Spawn;

    sun->setVisible(true);
    sun->setOpacity(1.0);
    sun->setPos(start);

    auto* animation = new QPropertyAnimation(sun, "pos", this);
    animation->setDuration(2000);
    animation->setStartValue(start);
    animation->setEndValue(end);
    animation->setEasingCurve(QEasingCurve::OutBounce);

    sunMotionAnimations.insert(sun, animation);

    QPointer<Sun> safeSun(sun);

    connect(animation, &QPropertyAnimation::finished, this, [this, safeSun, animation]() {
        if (safeSun && sunMotionAnimations.value(safeSun.data()) == animation) {
            sunMotionAnimations.remove(safeSun.data());
        }

        animation->deleteLater();

        if (safeSun && safeSun->isAlive()) {
            playSunIdleAnimation(safeSun.data());
        }
        });

    animation->start();
}

void AnimationManager::playSunSpawnAnimation(Sun* sun)
{
    if (!sun) {
        return;
    }

    const QPointF end = sun->pos();
    const QPointF start(end.x(), -60.0);

    playSunSpawnAnimation(sun, start, end);
}

void AnimationManager::playSunIdleAnimation(Sun* sun)
{
    if (!sun) {
        return;
    }

    if (sun->scene()) {
        lastKnownScene = sun->scene();
    }

    if (!animations.contains(sun)) {
        bindEntity(sun);
    }

    // 启动阳光序列帧 Idle。
    auto& table = animations[sun];
    if (table.contains(AnimationState::Idle) && table[AnimationState::Idle]) {
        table[AnimationState::Idle]->start();
    }

    currentStates[sun] = AnimationState::Idle;

    stopSunMotionAnimation(sun);

    const QPointF basePos = sun->pos();

    auto* floatAnimation = new QPropertyAnimation(sun, "pos", this);
    floatAnimation->setDuration(1000);
    floatAnimation->setStartValue(basePos);
    floatAnimation->setEndValue(basePos + QPointF(0, -10));
    floatAnimation->setEasingCurve(QEasingCurve::InOutSine);
    floatAnimation->setLoopCount(-1);

    sunMotionAnimations.insert(sun, floatAnimation);

    floatAnimation->start();
}

void AnimationManager::playSunCollectAnimation(Sun* sun, QPointF targetPos)
{
    if (!sun) {
        return;
    }

    if (sun->scene()) {
        lastKnownScene = sun->scene();
    }

    if (animations.contains(sun)) {
        auto& table = animations[sun];

        for (SpriteAnimation* animation : table) {
            if (animation) {
                animation->stop();
            }
        }
    }

    stopSunMotionAnimation(sun);

    currentStates[sun] = AnimationState::Collect;

    auto* opacityEffect = new QGraphicsOpacityEffect();
    sun->setGraphicsEffect(opacityEffect);

    auto* moveAnimation = new QPropertyAnimation(sun, "pos");
    moveAnimation->setDuration(500);
    moveAnimation->setStartValue(sun->pos());
    moveAnimation->setEndValue(targetPos);
    moveAnimation->setEasingCurve(QEasingCurve::InQuad);

    auto* fadeAnimation = new QPropertyAnimation(opacityEffect, "opacity");
    fadeAnimation->setDuration(500);
    fadeAnimation->setStartValue(1.0);
    fadeAnimation->setEndValue(0.0);

    auto* group = new QParallelAnimationGroup(this);
    group->addAnimation(moveAnimation);
    group->addAnimation(fadeAnimation);

    sunMotionAnimations.insert(sun, group);

    QPointer<Sun> safeSun(sun);

    connect(group, &QParallelAnimationGroup::finished, this, [this, safeSun, opacityEffect, group]() {
        if (safeSun && sunMotionAnimations.value(safeSun.data()) == group) {
            sunMotionAnimations.remove(safeSun.data());
        }

        if (safeSun) {
            safeSun->setGraphicsEffect(nullptr);
            safeSun->setVisible(false);

            emit sunCollectAnimationFinished(safeSun.data());
        }

        opacityEffect->deleteLater();
        group->deleteLater();
        });

    group->start();
}

void AnimationManager::playSunCollectAnimation(Sun* sun)
{
    playSunCollectAnimation(sun, defaultSunCollectTarget());
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
    if (!bullet) {
        return;
    }

    // Prompt 中 Bullet::Move 的简易版本允许使用单张 pea.png。
    // 子弹坐标移动仍由 Bullet::updateEntity() 负责，动画层只设置贴图和图层。
    QPixmap pixmap = ResourceManager::loadPixmap(":/images/bullets/pea.png");

    bullet->setPixmap(pixmap);
    bullet->setOffset(
        -pixmap.width() / 2.0,
        -pixmap.height() / 2.0
    );

    bullet->setZValue(BulletLayer);
    currentStates[bullet] = AnimationState::Move;
}

void AnimationManager::setupSunAnimations(Sun* sun)
{
    if (!sun) {
        return;
    }

    QVector<QPixmap> idleFrames =
        ResourceManager::loadFrames(":/images/sun", "sun", 3);

    addSpriteAnimation(
        sun,
        AnimationState::Idle,
        idleFrames,
        180,
        true
    );

    if (!idleFrames.isEmpty()) {
        sun->setPixmap(idleFrames.first());
        sun->setOffset(
            -idleFrames.first().width() / 2.0,
            -idleFrames.first().height() / 2.0
        );
    }

    sun->setZValue(SunLayer);
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
        QPointer<GameEntity> safeEntity(entity);

        connect(animation, &SpriteAnimation::finished, this, [this, safeEntity, state]() {
            if (!safeEntity) {
                return;
            }

            if (state == AnimationState::Death) {
                emit deathAnimationFinished(safeEntity.data());
                return;
            }

            currentStates[safeEntity.data()] = AnimationState::Idle;
            playAnimation(safeEntity.data(), AnimationState::Idle);
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

    case AnimationState::Spawn:
    case AnimationState::Collect:
    case AnimationState::Move:
        return 20;

    case AnimationState::Idle:
        return 10;
    }

    return 0;
}

void AnimationManager::stopSunMotionAnimation(Sun* sun)
{
    if (!sun) {
        return;
    }

    QAbstractAnimation* oldAnimation = sunMotionAnimations.take(sun);

    if (oldAnimation) {
        oldAnimation->stop();
        oldAnimation->deleteLater();
    }
}

QPointF AnimationManager::defaultSunCollectTarget() const
{
    // 当前仓库的 GameEngine::sunCollected 只有 Sun*，没有 SunDisplay 坐标。
    // 因此这里给一个左上角默认目标点。
    // 如果 UI 层能提供 SunDisplay 的 scene 坐标，应调用双参数版本。
    return QPointF(80.0, 40.0);
}
