#include "GameEngine.h"
#include <QGraphicsScene>
#include <iostream>
#include <ctime>

using namespace std;

GameEngine::GameEngine()
{
    gridManager = new GridManager();
    gameLoopTimer = new QTimer(this);
    connect(gameLoopTimer, &QTimer::timeout, this, &GameEngine::updateGame);
    sunValue=GameConfig::InitialSun;
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
    zombieMode = false;
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
    checkZombieAttackPlant();
    checkCollisions();
    for (auto* zombie : zombies)
        zombie->updateEntity();
    updateBullets();
    cleanupDeadEntities();
    for (auto it = plantCooldowns.begin(); it != plantCooldowns.end(); ++it) {
        int remaining = qMax(0, GameConfig::getPlantCd(it.key()) - it.value().elapsed());
        emit plantCooldownChanged(it.key(), remaining);
    }
    if (!zombieMode) checkWaveTransition();
    checkGameResult();
}

void GameEngine::start()
{
    running = true;
    currentWave = 0;
    waveCleared = false;
    gameLoopTimer->start(GameConfig::GameLoopInterval);
    if (!zombieMode) {
        sunGenerateTimer->start(GameConfig::SunGenerateInterval);
        startNextWave();
    }
}

void GameEngine::startZombieMode()
{
    zombieMode = true;
    sunValue = GameConfig::InitialSun;
    running = true;
    gameLoopTimer->start(GameConfig::GameLoopInterval);
    // 僵尸模式不需要天降阳光

    int plantCols = 4;
    for (int r = 0; r < GameConfig::Rows; ++r)
    {
        for (int c = 0; c < plantCols; ++c)
        {
            int roll = rand() % 100;
            // 前排多放射手，后排多放向日葵和坚果
            Plant* plant = nullptr;
            if (c <= 1) {
                // 前排：60% 射手, 20% 向日葵, 20% 坚果
                if (roll < 60)
                    plant = new Firefan(r, c);
                else if (roll < 80)
                    plant = new Kimsunflower(r, c);
                else
                    plant = new Bengbear(r, c);
            } else {
                // 后排：30% 射手, 40% 向日葵, 30% 坚果
                if (roll < 30)
                    plant = new Firefan(r, c);
                else if (roll < 70)
                    plant = new Kimsunflower(r, c);
                else
                    plant = new Bengbear(r, c);
            }

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

    if (getPlantCooldownRemaining(plantType) > 0)
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

    if (plantType == "Rainchili")
    {
        static_cast<Rainchili*>(plant)->startFuse();
        emit entityAnimationChanged(plant, AnimationState::Produce);
    }

    plantCooldowns[plantType].start();
    emit plantCooldownChanged(plantType, GameConfig::getPlantCd(plantType));

    return true;
}

bool GameEngine::removePlant(int row, int col)
{
    for (auto* plant : plants)
    {
        if (plant->getRow() == row && plant->getCol() == col)
        {
            plant->takeDamage(9999);
            return true;
        }
    }
    return false;
}

bool GameEngine::shovelPlant(int row, int col)
{
    for (int i = plants.size() - 1; i >= 0; --i)
    {
        Plant* plant = plants[i];
        if (plant->getRow() == row && plant->getCol() == col)
        {
            gridManager->removePlant(row, col);
            emit entityDied(plant);
            removeEntitySafely(plant);
            plants.removeAt(i);
            delete plant;
            return true;
        }
    }
    return false;
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
    if (zombieMode) return;
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
    if (dancing->reachedHome()) return;

    int baseRow = dancing->getRow();
    int baseCol = gridManager->scenePosToCell(dancing->pos()).x();

    struct { int dRow; int dCol; } offsets[] = {
        {-1,  0},
        { 1,  0},
        { 0, -1},
        { 0,  1},
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

    // 每次召唤完成后，舞王回到 Walk 状态。
    // 这样下一次 spawnTimer 触发时，AnimationState::Spawn 会重新播放，
    // 不会只在第一次召唤时跳舞。
    emit entityAnimationChanged(dancing, AnimationState::Walk);
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
            if (plants[i]->getPlantType() == "Kimsunflower")
            {
                Sun* sun = new Sun(GameConfig::FlowerSunValue);
                QPointF pos = plants[i]->pos();
                pos.setX(pos.x() + (rand() % 40 - 20));
                pos.setY(pos.y() + (rand() % 40 - 20));
                sun->setPos(pos);
                suns.append(sun);
                emit entityCreated(sun);
                connect(sun, &Sun::clicked, this, &GameEngine::collectSun);
            }
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

    if (auto* dancing = dynamic_cast<DancingZombie*>(zombie))
    {
        connect(dancing, &DancingZombie::summonAnimationStarted,
                this,
                [this](DancingZombie* self) {
                    emit entityAnimationChanged(self, AnimationState::Spawn);
                });

        connect(dancing, &DancingZombie::readyToSpawn,
                this, &GameEngine::onDancingZombieSpawn);
    }

    emit entityCreated(zombie);
    emit entityAnimationChanged(zombie, AnimationState::Walk);

    int baseInterval = 18000 - (currentWave - 1) * 5000;
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

int GameEngine::getPlantCooldownRemaining(const QString& type) const
{
    if (!plantCooldowns.contains(type)) return 0;
    int cd = GameConfig::getPlantCd(type);
    int elapsed = plantCooldowns[type].elapsed();
    return qMax(0, cd - elapsed);
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

        if (auto* dancing = dynamic_cast<DancingZombie*>(zombie))
        {
            if (dancing->isSummoning()) continue;
        }

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


                    // 僵尸模式下，向日葵每被咬三口掉落一个阳光
                    if (zombieMode && plantAhead->isAlive()
                        && plantAhead->getPlantType() == "Kimsunflower")
                    {
                        plantAhead->biteCount++;
                        if (plantAhead->biteCount % 3 == 0) {
                            Sun* sun = new Sun(GameConfig::FlowerSunValue);
                            QPointF pos = plantAhead->pos();
                            pos.setX(pos.x() + (rand() % 40 - 20));
                            pos.setY(pos.y() + (rand() % 40 - 20));
                            sun->setPos(pos);
                            suns.append(sun);
                            emit entityCreated(sun);
                            connect(sun, &Sun::clicked, this, &GameEngine::collectSun);
                        }
                    }


                    if (!plantAhead->isAlive())
                    {
                        emit entityDied(plantAhead);
                        gridManager->removePlant(row, plantAhead->getCol());
                        zombie->stopAttack();
                        emit entityAnimationChanged(zombie, AnimationState::Walk);
                    }
                }
            }
            else if (zombie->isAttacking())
            {
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
    if (zombieMode)
    {
        for (auto* p : plants)
        {
            if (p->isAlive()) return;
        }
        emit gameOver(true);
    }
    else
    {
        for (auto* z : zombies)
        {
            if (z->isAlive() && z->reachedHome())
            {
                emit gameOver(false);
                return;
            }
        }
    }
}

void GameEngine::checkWaveTransition()
{
    if (waveCleared) return;

    if (spawnedZombieCount < maxZombieCount) return;

    for (auto* z : zombies)
    {
        if (z->isAlive()) return;
    }

    waveCleared = true;
    zombieSpawnTimer->stop();

    if (currentWave >= totalWave)
    {
        emit gameOver(true);
        return;
    }

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

    switch (currentWave)
    {
    case 1: maxZombieCount = 6;  break;
    case 2: maxZombieCount = 12; break;
    case 3: maxZombieCount = 20; break;
    default: maxZombieCount = 6; break;
    }

    int initialInterval = (currentWave == 1) ? 18000 :
                          (currentWave == 2) ? 13000 : 8000;
    zombieSpawnTimer->start(initialInterval);

    emit waveChanged(currentWave, totalWave);
}

bool GameEngine::placeZombie(QString zombieType, int row, int col)
{
    if (!running) return false;
    if (!zombieMode) return false;
    if (col < 5 || col >= GameConfig::Cols) return false;

    if (!gridManager->isCellEmpty(row, col))
        return false;

    int cost = GameConfig::getZombieCost(zombieType);
    if (sunValue < cost)
        return false;

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

    emit sunChanged(sunValue);

    if (auto* dancing = dynamic_cast<DancingZombie*>(zombie))
    {
        connect(dancing, &DancingZombie::summonAnimationStarted,
                this,
                [this](DancingZombie* self) {
                    emit entityAnimationChanged(self, AnimationState::Spawn);
                });

        connect(dancing, &DancingZombie::readyToSpawn,
                this, &GameEngine::onDancingZombieSpawn);
    }

    emit entityCreated(zombie);
    emit entityAnimationChanged(zombie, AnimationState::Walk);

    return true;
}
