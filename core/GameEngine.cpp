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
    connect(sunGenerateTimer, &QTimer::timeout, this, &GameEngine::generateSun);

    zombieSpawnTimer = new QTimer(this);
    connect(zombieSpawnTimer, &QTimer::timeout, this, &GameEngine::spawnZombie);
    spawnedZombieCount = 0;
    maxZombieCount = 10;
    currentWave = 0;
    totalWave = 3;
    zombiesInCurrentWave = 0;
    waveCleared = false;
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
    checkPlantAttack();
    if (!zombieMode) checkSunProduction();
    checkRainchiliFuse();
    checkZombieAttackPlant();
    checkCollisions();
    for (auto* zombie : zombies)
        zombie->updateEntity();
    updateBullets();
    cleanupDeadEntities();
    if (!zombieMode) checkWaveTransition();
    checkGameResult();
}

void GameEngine::start()
{
    running = true;
    currentWave = 0;
    waveCleared = false;
    gameLoopTimer->start(GameConfig::GameLoopInterval);
    sunGenerateTimer->start(GameConfig::SunGenerateInterval);
    startNextWave();
}

void GameEngine::startZombieMode()
{
    zombieMode = true;
    sunValue = GameConfig::InitialSun;
    running = true;
    gameLoopTimer->start(GameConfig::GameLoopInterval);
    sunGenerateTimer->start(GameConfig::SunGenerateInterval);

    // 随机植物阵型：前四列随机放植物，每把都不一样
    int plantCols = 4;
    for (int r = 0; r < GameConfig::Rows; ++r)
    {
        for (int c = 0; c < plantCols; ++c)
        {
            int roll = rand() % 100;
            if (roll < 30) continue; // 30% 空格

            Plant* plant = nullptr;
            if (c <= 1 && roll < 70)
                plant = new Firefan(r, c);
            else if (roll < 60)
                plant = new Kimsunflower(r, c);
            else
                plant = new Bengbear(r, c);

            plant->setPos(gridManager->cellToScenePos(r, c));
            plants.append(plant);
            gridManager->placePlant(plant, r, c);
            emit entityCreated(plant);
        }
    }
}

void GameEngine::pause()
{
    if (!running) return;
    running = false;
    gameLoopTimer->stop();
    sunGenerateTimer->stop();
    zombieSpawnTimer->stop();
    for (auto* z : zombies)
    {
        if (auto* d = dynamic_cast<DancingZombie*>(z))
            d->stopSpawnTimer();
    }
}

void GameEngine::resume()
{
    if (running) return;
    running = true;
    gameLoopTimer->start();
    sunGenerateTimer->start();
    zombieSpawnTimer->start();
    for (auto* z : zombies)
    {
        if (auto* d = dynamic_cast<DancingZombie*>(z))
            d->startSpawnTimer();
    }
}

void GameEngine::stop()
{
    running = false;
    gameLoopTimer->stop();
    sunGenerateTimer->stop();
    zombieSpawnTimer->stop();
}

bool GameEngine::placePlant(QString plantType, int row, int col)
{
    if (!running) return false;
    if (!gridManager->isCellEmpty(row, col))
        return false;

    int cost = GameConfig::getPlantCost(plantType);
    if (sunValue < cost)
        return false;

    Plant* plant = nullptr;
    if (plantType == "Firefan")
        plant = new Firefan(row, col);
    else if (plantType == "Bengbear")
        plant = new Bengbear(row, col);
    else if (plantType == "Kimsunflower")
        plant = new Kimsunflower(row, col);
    else if (plantType == "Rainchili")
        plant = new Rainchili(row, col);
    else
        return false;

    sunValue -= cost;
    QPointF pos = gridManager->cellToScenePos(row, col);
    plant->setPos(pos);
    plants.append(plant);
    gridManager->placePlant(plant, row, col);
    emit sunChanged(sunValue);
    emit entityCreated(plant);

    // Rainchili：种下后蓄力，由 checkRainchiliFuse 延时引爆
    if (plantType == "Rainchili")
    {
        static_cast<Rainchili*>(plant)->startFuse();
        emit entityAnimationChanged(plant, AnimationState::Produce);
    }

    return true;
}

bool GameEngine::removePlant(int row, int col)
{
    if (!running) return false;

    Plant* plant = gridManager->getPlant(row, col);
    if (!plant || !plant->isAlive()) return false;

    plant->die();
    plant->markDeathAnimDone(); // 跳过死亡动画，直接标记可删除
    gridManager->removePlant(row, col);

    emit entityDied(plant);
    return true;
}

bool GameEngine::placeZombie(QString zombieType, int row, int col)
{
    if (!running || !zombieMode) return false;

    int cost = GameConfig::getZombieCost(zombieType);
    if (sunValue < cost) return false;

    // 僵尸只能放在右侧四列
    if (col < GameConfig::Cols - 4) return false;
    if (!gridManager->isValidCell(row, col)) return false;

    Zombie* zombie = nullptr;
    if (zombieType == "GenziZombie")
        zombie = new GenziZombie(row, col);
    else if (zombieType == "DancingZombie")
        zombie = new DancingZombie(row, col);
    else
        return false;

    sunValue -= cost;
    QPointF pos = gridManager->cellToScenePos(row, col);
    zombie->setPos(pos);
    zombies.append(zombie);

    if (auto* dancing = dynamic_cast<DancingZombie*>(zombie))
    {
        connect(dancing, &DancingZombie::readyToSpawn,
                this, &GameEngine::onDancingZombieSpawn);
    }

    emit sunChanged(sunValue);
    emit entityCreated(zombie);
    emit entityAnimationChanged(zombie, AnimationState::Walk);
    return true;
}

void GameEngine::checkPlantAttack()
{
    for (auto* plant : plants)
    {
        if (!plant->isAlive()) continue;
        if (!plant->canAttack(zombies) || !plant->readyToAttack())
            continue;

        emit entityAnimationChanged(plant, AnimationState::Attack);

        Bullet* b = plant->createBullet();
        if (b)
        {
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

void GameEngine::checkSunProduction()
{
    for (auto* plant : plants)
    {
        if (!plant->isAlive()) continue;
        if (plant->readyToProduceSun())
        {
            emit entityAnimationChanged(plant, AnimationState::Produce);
            Sun* s = plant->createSun();
            if (s)
            {
                suns.append(s);
                emit entityCreated(s);
                connect(s, &Sun::clicked, this, &GameEngine::collectSun);
            }
        }
    }
}

void GameEngine::checkRainchiliFuse()
{
    for (auto* plant : plants)
    {
        if (!plant->isAlive()) continue;
        if (plant->getPlantType() != "Rainchili") continue;

        auto* chili = static_cast<Rainchili*>(plant);
        if (!chili->readyToExplode()) continue;

        emit entityAnimationChanged(plant, AnimationState::Attack);

        for (auto* z : zombies)
        {
            if (z->isAlive() && z->getRow() == plant->getRow())
            {
                z->takeDamage(9999);
                if (!z->isAlive())
                    emit entityDied(z);
            }
        }

        plant->takeDamage(9999);
        if (!plant->isAlive())
        {
            gridManager->removePlant(plant->getRow(), plant->getCol());
            emit entityDied(plant);
        }
    }
}

void GameEngine::onDancingZombieSpawn(DancingZombie* dancing)
{
    if (!running) return;
    if (!dancing || !dancing->isAlive()) return;

    int baseRow = dancing->getRow();
    int baseCol = gridManager->scenePosToCell(dancing->pos()).x();

    struct { int dRow; int dCol; } offsets[] = {
        {-1,  0}, // 上
        { 1,  0}, // 下
        { 0, -1}, // 左
        { 0,  1}, // 右
    };

    for (auto& off : offsets)
    {
        int spawnRow = baseRow + off.dRow;
        int spawnCol = baseCol + off.dCol;

        if (!gridManager->isValidCell(spawnRow, spawnCol))
            continue;

        auto* dancer = new DancerZombie(spawnRow, spawnCol);
        dancer->setPos(gridManager->cellToScenePos(spawnRow, spawnCol));
        zombies.append(dancer);

        emit entityCreated(dancer);
        emit entityAnimationChanged(dancer, AnimationState::Walk);
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
                bullet->markDeathAnimDone();
                emit bulletHit(bullet->pos());
                if(!zombie->isAlive())
                {
                    emit entityDied(zombie);
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
        if (!bullets[i]->isAlive() && bullets[i]->isDeathAnimDone())
        {
            delete bullets[i];
            bullets.removeAt(i);
        }
    }
    for (int i = suns.size() - 1; i >= 0; --i)
    {
        if (!suns[i]->isAlive() && suns[i]->isDeathAnimDone())
        {
            delete suns[i];
            suns.removeAt(i);
        }
    }
    for (int i = zombies.size() - 1; i >= 0; --i)
    {
        if (!zombies[i]->isAlive() && zombies[i]->isDeathAnimDone())
        {
            delete zombies[i];
            zombies.removeAt(i);
        }
    }
    for (int i = plants.size() - 1; i >= 0; --i)
    {
        if (!plants[i]->isAlive() && plants[i]->isDeathAnimDone())
        {
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
    Sun* sun = new Sun(GameConfig::SkySunValue);
    QPointF pos = gridManager->cellToScenePos(r, c);
    sun->setPos(pos);
    suns.append(sun);
    emit sunCreated(sun);
    emit entityCreated(sun);
    connect(sun, &Sun::clicked, this, &GameEngine::collectSun);
}

void GameEngine::spawnZombie()
{
    if (spawnedZombieCount >= maxZombieCount || waveCleared)
        return;

    int row = rand() % GameConfig::Rows;

    // 每波僵尸构成不同：Wave 1 纯普通，后面波次混入舞王
    Zombie* zombie = nullptr;
    if (currentWave == 1)
    {
        zombie = new GenziZombie(row, GameConfig::Cols - 1);
    }
    else
    {
        zombie = (rand() % 3 == 0)
                     ? static_cast<Zombie*>(new DancingZombie(row, GameConfig::Cols - 1))
                     : static_cast<Zombie*>(new GenziZombie(row, GameConfig::Cols - 1));
    }

    QPointF pos = gridManager->cellToScenePos(row, GameConfig::Cols - 1);
    zombie->setPos(pos);
    zombies.append(zombie);
    spawnedZombieCount++;

    // 舞王僵尸：连接独立的生成信号
    if (auto* dancing = dynamic_cast<DancingZombie*>(zombie))
    {
        connect(dancing, &DancingZombie::readyToSpawn,
                this, &GameEngine::onDancingZombieSpawn);
    }

    emit entityCreated(zombie);
    emit entityAnimationChanged(zombie, AnimationState::Walk);

    // 波内渐进加速：第一只最慢，越往后越快
    int baseInterval = 18000 - (currentWave - 1) * 5000;  // Wave1:18s Wave2:13s Wave3:8s
    int newInterval = qMax(2000, baseInterval - spawnedZombieCount * 1500);
    zombieSpawnTimer->setInterval(newInterval);
}

void GameEngine::collectSun(Sun* sun)
{
    if(!sun || !sun->isAlive()) return;
    if(!running) return;
    sunValue += sun->getValue();
    sun->collect();
    emit sunChanged(sunValue);
    emit sunCollected(sun);
}

void GameEngine::removeEntitySafely(GameEntity* entity)
{
    if (!entity) return;

    entity->markDeathAnimDone();

    if (entity->scene()) {
        entity->scene()->removeItem(entity);
    }
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
            if (dx >= 0 && dx < 60)
            {
                if (!zombie->isAttacking())
                {
                    zombie->startAttack(plantAhead);
                    emit entityAnimationChanged(zombie, AnimationState::Attack);
                }

                if (zombie->isAttacking() && zombie->readyToDealDamage())
                {
                    zombie->resetAttackTimer();
                    plantAhead->takeDamage(zombie->getAttackDamage());

                    // 僵尸模式：向日葵被咬掉阳光
                    if (zombieMode && plantAhead->getPlantType() == "Kimsunflower")
                    {
                        Sun* drop = new Sun(GameConfig::FlowerSunValue);
                        drop->setPos(plantAhead->pos() + QPointF(rand() % 40 - 20, -20));
                        suns.append(drop);
                        emit entityCreated(drop);
                        connect(drop, &Sun::clicked, this, &GameEngine::collectSun);
                    }

                    if (!plantAhead->isAlive())
                    {
                        emit entityDied(plantAhead);
                        // 植物死亡后立即从网格移除，防止后续帧误判
                        gridManager->removePlant(row, plantAhead->getCol());
                        zombie->stopAttack();
                        emit entityAnimationChanged(zombie, AnimationState::Walk);
                    }
                }
            }
            else if (zombie->isAttacking())
            {
                // 有植物但距离不够 → 僵尸不应保持攻击状态（目标可能刚死，更远的植物还未接近）
                zombie->stopAttack();
                emit entityAnimationChanged(zombie, AnimationState::Walk);
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

    // 僵尸模式：植物全灭即胜利
    if (zombieMode)
    {
        for (auto* p : plants)
        {
            if (p->isAlive()) return;
        }
        emit gameOver(true);
    }
}

void GameEngine::checkWaveTransition()
{
    if (waveCleared) return;

    // 当前波次僵尸还没刷完
    if (spawnedZombieCount < maxZombieCount) return;

    // 还有活着的僵尸
    for (auto* z : zombies)
    {
        if (z->isAlive()) return;
    }

    // 当前波次清除
    waveCleared = true;
    zombieSpawnTimer->stop();

    if (currentWave >= totalWave)
    {
        // 所有波次完成，胜利
        emit gameOver(true);
        return;
    }

    // 波间停顿后开始下一波
    QTimer::singleShot(5000, this, [this]() {
        if (running)
            startNextWave();
    });
}

void GameEngine::startNextWave()
{
    currentWave++;
    waveCleared = false;
    spawnedZombieCount = 0;

    // 每波僵尸数量递增，后期僵尸潮
    switch (currentWave)
    {
    case 1: maxZombieCount = 6;  break;
    case 2: maxZombieCount = 12; break;
    case 3: maxZombieCount = 20; break;
    default: maxZombieCount = 6; break;
    }

    // 初始间隔长，给玩家部署时间，波内由 spawnZombie 渐进加速
    int initialInterval = (currentWave == 1) ? 18000 :
                          (currentWave == 2) ? 13000 : 8000;
    zombieSpawnTimer->start(initialInterval);

    emit waveChanged(currentWave, totalWave);
}