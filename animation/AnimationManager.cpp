#include "AnimationManager.h"

#include "ResourceManager.h"
#include "SpriteAnimation.h"
#include "EffectItem.h"

#include "core/GameEntity.h"
#include "core/Plant.h"
#include "core/Zombie.h"
#include "core/Bullet.h"
#include "core/Sun.h"

#include <QAbstractAnimation>
#include <QDebug>
#include <QEasingCurve>
#include <QGraphicsOpacityEffect>
#include <QGraphicsScene>
#include <QParallelAnimationGroup>
#include <QPointer>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>

namespace {

const QString ImageRoot = QStringLiteral(":/images");

QString normalizeResourceKey(QString key)
{
    key = key.toLower().trimmed();

    if (key.contains(QStringLiteral("genzizombie")) ||
        key.contains(QStringLiteral("genzi"))) {
        return QStringLiteral("genzizombie");
    }

    if (key.contains(QStringLiteral("dancingzombie")) ||
        key.contains(QStringLiteral("dancing"))) {
        return QStringLiteral("dancingzombie");
    }

    // 伴舞僵尸当前没有独立素材，复用普通僵尸素材，保证舞王召唤后有动画。
    if (key.contains(QStringLiteral("dancerzombie")) ||
        key.contains(QStringLiteral("dancer"))) {
        return QStringLiteral("chicken");
    }

    if (key.contains(QStringLiteral("eater"))) {
        return QStringLiteral("eater");
    }

    if (key.contains(QStringLiteral("chicken"))) {
        return QStringLiteral("chicken");
    }

    if (key.contains(QStringLiteral("kun"))) {
        return QStringLiteral("kun");
    }

    if (key.contains(QStringLiteral("bengbear"))) {
        return QStringLiteral("bengbear");
    }

    if (key.contains(QStringLiteral("kimsunflower")) ||
        key.contains(QStringLiteral("sunflower"))) {
        return QStringLiteral("kimsunflower");
    }

    if (key.contains(QStringLiteral("firefan"))) {
        return QStringLiteral("firefan");
    }

    if (key.contains(QStringLiteral("rainchili")) ||
        key.contains(QStringLiteral("chili"))) {
        return QStringLiteral("rainchili");
    }

    return key;
}

QString plantResourceKey(Plant* plant)
{
    if (!plant) {
        return QStringLiteral("bengbear");
    }

    const QString typeKey = normalizeResourceKey(plant->getPlantType());
    if (!typeKey.isEmpty()) {
        return typeKey;
    }

    const QString objectKey = normalizeResourceKey(plant->objectName());
    if (!objectKey.isEmpty()) {
        return objectKey;
    }

    return QStringLiteral("bengbear");
}

QString zombieResourceKey(Zombie* zombie)
{
    if (!zombie) {
        return QStringLiteral("genzizombie");
    }

    if (dynamic_cast<DancingZombie*>(zombie)) {
        return QStringLiteral("dancingzombie");
    }

    if (dynamic_cast<DancerZombie*>(zombie)) {
        return QStringLiteral("chicken");
    }

    if (dynamic_cast<GenziZombie*>(zombie)) {
        return QStringLiteral("genzizombie");
    }

    const QString dataKey = normalizeResourceKey(zombie->data(0).toString());
    if (!dataKey.isEmpty()) {
        return dataKey;
    }

    const QString objectKey = normalizeResourceKey(zombie->objectName());
    if (!objectKey.isEmpty()) {
        return objectKey;
    }

    return QStringLiteral("eater");
}

QString entityResourceKey(GameEntity* entity)
{
    if (!entity) {
        return QString();
    }

    switch (entity->getEntityType()) {
    case EntityType::Plant:
        return plantResourceKey(static_cast<Plant*>(entity));
    case EntityType::Zombie:
        return zombieResourceKey(static_cast<Zombie*>(entity));
    case EntityType::Bullet:
        return QStringLiteral("bullets");
    case EntityType::Sun:
        return QStringLiteral("other");
    case EntityType::Effect:
        return QStringLiteral("effects");
    }

    return QString();
}

QString entityFolder(GameEntity* entity)
{
    const QString key = entityResourceKey(entity);
    if (key.isEmpty()) {
        return ImageRoot + QStringLiteral("/unknown");
    }

    return ImageRoot + QStringLiteral("/") + key;
}

int plantIdleFrameCount(const QString& key)
{
    if (key == QStringLiteral("bengbear")) {
        return 8;
    }

    if (key == QStringLiteral("kimsunflower")) {
        return 4;
    }

    if (key == QStringLiteral("firefan")) {
        return 8;
    }

    if (key == QStringLiteral("rainchili")) {
        return 1;
    }

    return 1;
}

int plantProduceFrameCount(const QString& key)
{
    if (key == QStringLiteral("kimsunflower")) {
        return 7;
    }

    return 0;
}

int zombieWalkFrameCount(const QString& key)
{
    if (key == QStringLiteral("genzizombie") ||
        key == QStringLiteral("dancingzombie") ||
        key == QStringLiteral("eater") ||
        key == QStringLiteral("kun")) {
        return 8;
    }

    if (key == QStringLiteral("chicken")) {
        return 4;
    }

    return 1;
}

int zombieAttackFrameCount(const QString& key)
{
    if (key == QStringLiteral("genzizombie") ||
        key == QStringLiteral("dancingzombie") ||
        key == QStringLiteral("eater") ||
        key == QStringLiteral("kun")) {
        return 6;
    }

    if (key == QStringLiteral("chicken")) {
        return 5;
    }

    return 1;
}

int zombieDeathFrameCount(const QString& key)
{
    if (key == QStringLiteral("genzizombie") ||
        key == QStringLiteral("dancingzombie") ||
        key == QStringLiteral("eater") ||
        key == QStringLiteral("kun")) {
        return 8;
    }

    if (key == QStringLiteral("chicken")) {
        return 5;
    }

    return 1;
}

bool canReleaseHighPriorityState(AnimationState oldState, AnimationState newState)
{
    if (oldState == AnimationState::Death) {
        return false;
    }

    // Attack / Produce 是循环或短动作。目标消失后，后端会发 Walk / Idle，必须允许退回。
    if ((oldState == AnimationState::Attack || oldState == AnimationState::Produce) &&
        (newState == AnimationState::Walk || newState == AnimationState::Idle)) {
        return true;
    }

    return false;
}

} // namespace

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
        playAnimation(entity, AnimationState::Idle);
        break;

    case EntityType::Zombie:
        setupZombieAnimations(static_cast<Zombie*>(entity));
        playAnimation(entity, AnimationState::Walk);
        break;

    case EntityType::Bullet:
        setupBulletAnimations(static_cast<Bullet*>(entity));
        playAnimation(entity, AnimationState::Move);
        break;

    case EntityType::Sun:
        setupSunAnimations(static_cast<Sun*>(entity));
        // 不依赖 UI 额外连接 sunCreated，也能让普通生成、向日葵生成、向日葵被咬掉落都进入 Spawn。
        playSunSpawnAnimation(static_cast<Sun*>(entity));
        break;

    case EntityType::Effect:
        break;
    }

    const bool isSun = entity->getEntityType() == EntityType::Sun;
    Sun* sunKey = isSun ? static_cast<Sun*>(entity) : nullptr;

    connect(entity, &QObject::destroyed, this, [this, entity, sunKey]() {
        auto entityAnimations = animations.take(entity);
        for (SpriteAnimation* animation : entityAnimations) {
            if (animation) {
                animation->stop();
                animation->deleteLater();
            }
        }

        if (sunKey) {
            QAbstractAnimation* motion = sunMotionAnimations.take(sunKey);
            if (motion) {
                motion->stop();
                motion->deleteLater();
            }
        }

        currentStates.remove(entity);
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

    stopEntitySpriteAnimations(entity);

    auto entityAnimations = animations.take(entity);
    for (SpriteAnimation* animation : entityAnimations) {
        if (animation) {
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

    if (entity->scene()) {
        lastKnownScene = entity->scene();
    }

    if (!animations.contains(entity)) {
        bindEntity(entity);
    }

    if (entity->getEntityType() == EntityType::Sun) {
        auto* sun = static_cast<Sun*>(entity);
        if (state == AnimationState::Spawn) {
            playSunSpawnAnimation(sun);
            return;
        }
        if (state == AnimationState::Collect) {
            playSunCollectAnimation(sun);
            return;
        }
    }

    if (entity->getEntityType() == EntityType::Bullet && state == AnimationState::Hit) {
        playBulletHitEffect(entity->pos());
        return;
    }

    const AnimationState oldState = currentStates.value(entity, AnimationState::Idle);

    if (oldState == AnimationState::Death) {
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

    const bool releaseTransition = canReleaseHighPriorityState(oldState, state);
    if (!releaseTransition && animationPriority(state) < animationPriority(oldState)) {
        return;
    }

    auto& table = animations[entity];

    if (!table.contains(state)) {
        qWarning() << "AnimationManager: missing animation state:"
                   << static_cast<int>(state)
                   << "entity key:"
                   << entityResourceKey(entity);
        return;
    }

    SpriteAnimation* nextAnimation = table.value(state, nullptr);
    if (!nextAnimation) {
        return;
    }

    if (oldState == state && nextAnimation->isRunning()) {
        return;
    }

    if (table.contains(oldState) && table[oldState]) {
        table[oldState]->stop();
    }

    currentStates[entity] = state;

    nextAnimation->reset();
    nextAnimation->start();
}

void AnimationManager::playHitEffect(GameEntity* entity)
{
    if (!entity) {
        return;
    }

    if (currentStates.value(entity, AnimationState::Idle) == AnimationState::Death) {
        return;
    }

    if (entity->getEntityType() == EntityType::Sun) {
        return;
    }

    auto* effect = new QGraphicsOpacityEffect();
    entity->setGraphicsEffect(effect);

    auto* fadeOut = new QPropertyAnimation(effect, "opacity");
    fadeOut->setDuration(80);
    fadeOut->setStartValue(1.0);
    fadeOut->setEndValue(0.35);

    auto* fadeIn = new QPropertyAnimation(effect, "opacity");
    fadeIn->setDuration(120);
    fadeIn->setStartValue(0.35);
    fadeIn->setEndValue(1.0);

    auto* group = new QSequentialAnimationGroup(this);
    group->addAnimation(fadeOut);
    group->addAnimation(fadeIn);

    QPointer<GameEntity> safeEntity(entity);
    connect(group, &QSequentialAnimationGroup::finished, this,
            [safeEntity, group]() {
                if (safeEntity) {
                    safeEntity->setGraphicsEffect(nullptr);
                }
                group->deleteLater();
            });

    group->start();
}

void AnimationManager::playDeathEffect(GameEntity* entity)
{
    if (!entity) {
        return;
    }

    if (currentStates.value(entity, AnimationState::Idle) == AnimationState::Death) {
        return;
    }

    currentStates[entity] = AnimationState::Death;

    if (entity->getEntityType() == EntityType::Sun) {
        stopSunMotionAnimation(static_cast<Sun*>(entity));
    }

    stopEntitySpriteAnimations(entity);

    if (animations.contains(entity)) {
        auto& table = animations[entity];
        SpriteAnimation* deathAnimation = table.value(AnimationState::Death, nullptr);

        if (deathAnimation) {
            deathAnimation->reset();
            deathAnimation->start();
            return;
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
    connect(animation, &QPropertyAnimation::finished, this,
            [this, safeEntity, animation]() {
                if (safeEntity) {
                    safeEntity->setGraphicsEffect(nullptr);
                    emit deathAnimationFinished(safeEntity.data());
                }
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
        qWarning() << "AnimationManager: no scene cached for bullet hit effect.";
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

    if (!animations.contains(sun)) {
        bindEntity(sun);
    }

    stopSunMotionAnimation(sun);

    currentStates[sun] = AnimationState::Spawn;

    sun->setVisible(true);
    sun->setOpacity(1.0);
    sun->setGraphicsEffect(nullptr);
    sun->setAcceptedMouseButtons(Qt::LeftButton);
    sun->setZValue(SunLayer);
    sun->setPos(start);

    auto* animation = new QPropertyAnimation(sun, "pos", this);
    animation->setDuration(900);
    animation->setStartValue(start);
    animation->setEndValue(end);
    animation->setEasingCurve(QEasingCurve::OutBounce);

    sunMotionAnimations.insert(sun, animation);

    QPointer<Sun> safeSun(sun);
    connect(animation, &QPropertyAnimation::finished, this,
            [this, safeSun, animation]() {
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
    const QPointF start = end + QPointF(0.0, -120.0);
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

    stopSunMotionAnimation(sun);

    auto& table = animations[sun];
    SpriteAnimation* idleAnimation = table.value(AnimationState::Idle, nullptr);

    if (idleAnimation && !idleAnimation->isRunning()) {
        idleAnimation->reset();
        idleAnimation->start();
    }

    currentStates[sun] = AnimationState::Idle;

    const QPointF basePos = sun->pos();

    auto* upAnimation = new QPropertyAnimation(sun, "pos");
    upAnimation->setDuration(700);
    upAnimation->setStartValue(basePos);
    upAnimation->setEndValue(basePos + QPointF(0, -8));
    upAnimation->setEasingCurve(QEasingCurve::InOutSine);

    auto* downAnimation = new QPropertyAnimation(sun, "pos");
    downAnimation->setDuration(700);
    downAnimation->setStartValue(basePos + QPointF(0, -8));
    downAnimation->setEndValue(basePos);
    downAnimation->setEasingCurve(QEasingCurve::InOutSine);

    auto* floatGroup = new QSequentialAnimationGroup(this);
    floatGroup->addAnimation(upAnimation);
    floatGroup->addAnimation(downAnimation);
    floatGroup->setLoopCount(-1);

    sunMotionAnimations.insert(sun, floatGroup);
    floatGroup->start();
}

void AnimationManager::playSunCollectAnimation(Sun* sun, QPointF targetPos)
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

    stopEntitySpriteAnimations(sun);
    stopSunMotionAnimation(sun);

    currentStates[sun] = AnimationState::Collect;

    sun->setAcceptedMouseButtons(Qt::NoButton);
    sun->setVisible(true);
    sun->setOpacity(1.0);
    sun->setZValue(SunLayer);

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
    connect(group, &QParallelAnimationGroup::finished, this,
            [this, safeSun, group]() {
                if (safeSun && sunMotionAnimations.value(safeSun.data()) == group) {
                    sunMotionAnimations.remove(safeSun.data());
                }

                if (safeSun) {
                    safeSun->setGraphicsEffect(nullptr);
                    safeSun->setVisible(false);
                    emit sunCollectAnimationFinished(safeSun.data());
                }

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

    const QString key = plantResourceKey(plant);
    const QString folder = entityFolder(plant);

    QVector<QPixmap> idleFrames =
        ResourceManager::loadFrames(folder, "idle", plantIdleFrameCount(key));

    addSpriteAnimation(
        plant,
        AnimationState::Idle,
        idleFrames,
        180,
        true
        );

    // 射手攻击、坚果被动、辣椒蓄力都至少有可播放的短动作，避免状态缺失。
    addSpriteAnimation(
        plant,
        AnimationState::Attack,
        idleFrames,
        120,
        false
        );

    const int produceFrameCount = plantProduceFrameCount(key);
    if (produceFrameCount > 0) {
        QVector<QPixmap> produceFrames =
            ResourceManager::loadFrames(folder, "produce", produceFrameCount);

        addSpriteAnimation(
            plant,
            AnimationState::Produce,
            produceFrames,
            120,
            false
            );
    }

    if (!idleFrames.isEmpty()) {
        plant->setPixmap(idleFrames.first());
        plant->setOffset(
            -idleFrames.first().width() / 2.0,
            -idleFrames.first().height() / 2.0
            );
    }

    plant->setZValue(PlantLayer + plant->pos().y() / 1000.0);
    currentStates[plant] = AnimationState::Idle;
}

void AnimationManager::setupZombieAnimations(Zombie* zombie)
{
    if (!zombie) {
        return;
    }

    const QString key = zombieResourceKey(zombie);
    const QString folder = entityFolder(zombie);

    QVector<QPixmap> walkFrames =
        ResourceManager::loadFrames(folder, "walk", zombieWalkFrameCount(key));

    QVector<QPixmap> attackFrames =
        ResourceManager::loadFrames(folder, "attack", zombieAttackFrameCount(key));

    QVector<QPixmap> deathFrames =
        ResourceManager::loadFrames(folder, "death", zombieDeathFrameCount(key));

    addSpriteAnimation(
        zombie,
        AnimationState::Walk,
        walkFrames,
        160,
        true
        );

    addSpriteAnimation(
        zombie,
        AnimationState::Attack,
        attackFrames,
        140,
        true
        );

    addSpriteAnimation(
        zombie,
        AnimationState::Death,
        deathFrames,
        120,
        false
        );

    if (!walkFrames.isEmpty()) {
        zombie->setPixmap(walkFrames.first());
        zombie->setOffset(
            -walkFrames.first().width() / 2.0,
            -walkFrames.first().height() / 2.0
            );
    }

    zombie->setZValue(ZombieLayer + zombie->pos().y() / 1000.0);
    currentStates[zombie] = AnimationState::Walk;
}

void AnimationManager::setupBulletAnimations(Bullet* bullet)
{
    if (!bullet) {
        return;
    }

    QPixmap pixmap = ResourceManager::loadPixmap(":/images/bullets/pea.png");
    QVector<QPixmap> moveFrames;
    moveFrames.append(pixmap);

    addSpriteAnimation(
        bullet,
        AnimationState::Move,
        moveFrames,
        100,
        true
        );

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

    QVector<QPixmap> idleFrames;
    idleFrames.append(ResourceManager::loadPixmap(":/images/other/sunlight.png"));

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

    sun->setAcceptedMouseButtons(Qt::LeftButton);
    sun->setZValue(SunLayer);
    currentStates[sun] = AnimationState::Idle;
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

        connect(animation, &SpriteAnimation::finished, this,
                [this, safeEntity, state]() {
                    if (!safeEntity) {
                        return;
                    }

                    if (state == AnimationState::Death) {
                        emit deathAnimationFinished(safeEntity.data());
                        return;
                    }

                    AnimationState fallback = AnimationState::Idle;

                    if (safeEntity->getEntityType() == EntityType::Zombie) {
                        fallback = AnimationState::Walk;
                    }

                    currentStates[safeEntity.data()] = fallback;
                    playAnimation(safeEntity.data(), fallback);
                });
    }
}

void AnimationManager::stopEntitySpriteAnimations(GameEntity* entity)
{
    if (!entity || !animations.contains(entity)) {
        return;
    }

    auto& table = animations[entity];
    for (SpriteAnimation* animation : table) {
        if (animation) {
            animation->stop();
        }
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
    // 当前 TopBar/SunDisplay 位于左上区域；UI 若能拿到精确 scene 坐标，可调用双参数版本覆盖。
    return QPointF(80.0, 40.0);
}