#include "GameEngine.h"
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
    for(auto*zombie:zombies)
        zombie->updateEntity();// 僵尸走
    updateBullets();// 子弹飞
    checkCollisions();// 子弹撞僵尸
    cleanupDeadEntities();// 最后：收尸
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
    for (int i = bullets.size() - 1; i >= 0; --i)
    {
        // 清理子弹
        if (!bullets[i]->isAlive())
        {
            delete bullets[i];
            bullets.removeAt(i);
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