#include "GameEngine.h"
#include <QGraphicsScene>
#include <iostream>

using namespace std;

GameEngine::GameEngine()
{
    gridManager = new GridManager();
    gameLoopTimer = new QTimer(this);
    connect(gameLoopTimer, &QTimer::timeout, this, &GameEngine::updateGame);
    sunValue=GameConfig::InitialSun;// 150
    sunGenerateTimer = new QTimer(this);
    connect(sunGenerateTimer, &QTimer::timeout,this,&GameEngine::generateSun);
}

void GameEngine::addPlant(Plant* plant)
{
    plants.append(plant);
}

void GameEngine::addZombie(Zombie* zombie)
{
    zombies.append(zombie);
}

void GameEngine::updateGame()
{
    checkPlantAttack();// 先：植物攻击
    checkZombieAttackPlant();
    checkCollisions();
    for (auto* zombie : zombies)
        zombie->updateEntity();
    updateBullets();
    cleanupDeadEntities();
    checkGameResult();
}

void GameEngine::start()
{
    gameLoopTimer->start(GameConfig::GameLoopInterval);
    sunGenerateTimer->start(GameConfig::SunGenerateInterval);
}

bool GameEngine::placePlant(QString plantType, int row, int col)
{
    if (!gridManager->isCellEmpty(row, col))
    {
        return false;
    }
    Plant* plant = new Plant(row, col, 300, 0, plantType);
    QPointF pos = gridManager->cellToScenePos(row, col);
    plant->setPos(pos);
    plants.append(plant);
    gridManager->placePlant(plant, row, col);
    emit entityCreated(plant);
    return true;
}

void GameEngine::checkPlantAttack()
{
    for (auto* plant : plants)
    {
        if (!plant->isAlive()) continue;
        if (plant->canAttack(zombies) && plant->readyToAttack())
        {
            emit entityAnimationChanged(plant, AnimationState::Attack);
            Bullet* b = plant->createBullet();
            bullets.append(b);
            emit entityCreated(b);
        }
    }
}

void GameEngine::updateBullets()
{
    for (auto* bullet : bullets)
    {
        if (bullet->isAlive())
        {
            bullet->updateEntity();
        }
    }
}

void GameEngine::checkCollisions()
{
    for (auto* bullet : bullets)
    {
        if (!bullet->isAlive()) continue;
        for (auto* zombie : zombies)
        {
            if (!zombie->isAlive()) continue;
            if (bullet->collideWithZombie(zombie))
            {
                zombie->takeDamage(bullet->getDamage());
                bullet->die();
                emit bulletHit(bullet->pos());
                if(!zombie->isAlive())
                {
                    emit entityDied(zombie);
                    // 从 GridManager 移除（如果有植物在它面前）
                }
                break;
            }
        }
    }
}

void GameEngine::cleanupDeadEntities()
{
    // 子弹
    for (int i = bullets.size() - 1; i >= 0; --i)
    {
        if (!bullets[i]->isAlive())
        {
            delete bullets[i];
            bullets.removeAt(i);
        }
    }
    //阳光
    for (int i = suns.size() - 1; i >= 0; --i)
    {
        if(!suns[i]->isAlive())
        {
            delete suns[i];
            suns.removeAt(i);
        }
    }
    // 僵尸
    for (int i = zombies.size() - 1; i >= 0; --i)
    {
        if (!zombies[i]->isAlive()) {
            delete zombies[i];
            zombies.removeAt(i);
        }
    }
    // 植物
    for (int i = plants.size() - 1; i >= 0; --i){
        if(!plants[i]->isAlive()){
            gridManager->removePlant(plants[i]->getRow(), plants[i]->getCol());
            delete plants[i];
            plants.removeAt(i);
        }
    }
}

void GameEngine::generateSun()
{
    int r = rand() % GameConfig::Rows;
    int c = rand() % GameConfig::Cols;
    Sun* sun = new Sun(25);
    QPointF pos = gridManager->cellToScenePos(r, c);
    sun->setPos(pos);
    suns.append(sun);
    emit sunCreated(sun);
    emit entityCreated(sun);
    connect(sun, &Sun::clicked, this, &GameEngine::collectSun);
}

void GameEngine::collectSun(Sun* sun)
{
    if(!sun || !sun->isAlive()) return;
    sunValue += sun->getValue();
    sun->collect();
    emit sunChanged(sunValue);
    emit sunCollected(sun);
}

void GameEngine::removeEntitySafely(GameEntity* entity)
{
    if (!entity) return;

    // 从场景移除（如果还在）
    if (entity->scene()) {
        entity->scene()->removeItem(entity);
    }

    // 不 delete — 等 cleanupDeadEntities 统一处理
}

void GameEngine::checkZombieAttackPlant()
{
    for (auto* zombie : zombies)
    {
        if (!zombie->isAlive()) continue;
        int row = zombie->getRow();
        QPoint cell = gridManager->scenePosToCell(zombie->pos());
        int col = cell.x();
        Plant* plantAhead = nullptr;
        for (int c = col; c >= 0; --c)
        {
            plantAhead = gridManager->getPlant(row, c);
            if (plantAhead && plantAhead->isAlive()) break;
            plantAhead = nullptr;
        }
        if (plantAhead)
        {
            qreal dx = zombie->pos().x() - plantAhead->pos().x();
            if (dx > 0 && dx < 60)
            {
                if (!zombie->isAttacking())
                {
                    zombie->startAttack(plantAhead);
                    emit entityAnimationChanged(zombie, AnimationState::Attack);
                }
            }
        }
        else
        {
            if (zombie->isAttacking())
            {
                zombie->stopAttack();
                emit entityAnimationChanged(zombie, AnimationState::Walk);
            }
        }
    }
}

void GameEngine::checkGameResult()
{
    for (auto* z : zombies)
    {
        if (z->isAlive() && z->reachedHome())
        {
            emit gameOver(false);
            return;
        }
    }
    bool allDead = true;
    for (auto* z : zombies)
    {
        if (z->isAlive()) { allDead = false; break; }
    }
    if (allDead && !zombies.isEmpty())
    {
        emit gameOver(true);
    }
}