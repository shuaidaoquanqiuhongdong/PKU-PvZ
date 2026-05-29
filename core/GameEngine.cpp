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
    checkSunProduction();
    checkRainchiliFuse();
    checkDancingZombieSpawn();
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
    running = true;
    gameLoopTimer->start(GameConfig::GameLoopInterval);
    sunGenerateTimer->start(GameConfig::SunGenerateInterval);
    zombieSpawnTimer->start(GameConfig::ZombieSpawnInterval);
}

void GameEngine::pause()
{
    if (!running) return;
    running = false;
    gameLoopTimer->stop();
    sunGenerateTimer->stop();
    zombieSpawnTimer->stop();
}

void GameEngine::resume()
{
    if (running) return;
    running = true;
    gameLoopTimer->start();
    sunGenerateTimer->start();
    zombieSpawnTimer->start();
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

void GameEngine::checkDancingZombieSpawn()
{
    // 先收集需要生成的舞王僵尸，避免遍历时修改列表
    QList<DancingZombie*> spawners;

    for (auto* zombie : zombies)
    {
        if (!zombie->isAlive()) continue;

        auto* dancing = dynamic_cast<DancingZombie*>(zombie);
        if (!dancing) continue;
        if (!dancing->readyToSpawn()) continue;

        dancing->resetSpawnTimer();
        spawners.append(dancing);
    }

    // 上下左右各生成一只伴舞僵尸（放在对应格子的中心）
    struct { int dRow; int dCol; } offsets[] = {
        {-1,  0}, // 上
        { 1,  0}, // 下
        { 0, -1}, // 左
        { 0,  1}, // 右
    };

    for (auto* dancing : spawners)
    {
        int baseRow = dancing->getRow();
        int baseCol = gridManager->scenePosToCell(dancing->pos()).x();

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
    if (spawnedZombieCount >= maxZombieCount)
        return;

    int row = rand() % GameConfig::Rows;

    Zombie* zombie = nullptr;
    if (spawnedZombieCount < 5)
        zombie = new GenziZombie(row, GameConfig::Cols - 1);
    else
        zombie = (rand() % 2 == 0)
                     ? static_cast<Zombie*>(new GenziZombie(row, GameConfig::Cols - 1))
                     : static_cast<Zombie*>(new DancingZombie(row, GameConfig::Cols - 1));

    QPointF pos = gridManager->cellToScenePos(row, GameConfig::Cols - 1);
    zombie->setPos(pos);
    zombies.append(zombie);
    spawnedZombieCount++;

    emit entityCreated(zombie);
    emit entityAnimationChanged(zombie, AnimationState::Walk);

    // 渐进加速：初始10s，每生一只缩短300ms，最快3s
    int newInterval = qMax(3000, GameConfig::ZombieSpawnInterval - spawnedZombieCount * 300);
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
            if (dx > 0 && dx < 60)
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
    bool allDead = true;
    for (auto* z : zombies)
    {
        if (z->isAlive()) { allDead = false; break; }
    }
    if (allDead && spawnedZombieCount >= maxZombieCount)
    {
        emit gameOver(true);
    }
}