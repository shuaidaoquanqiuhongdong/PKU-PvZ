#include "GameEngine.h"
#include <iostream>

using namespace std;

GameEngine::GameEngine()
{
    gridManager = new GridManager();
    gameLoopTimer = new QTimer(this);
    connect(gameLoopTimer, &QTimer::timeout, this, &GameEngine::updateGame);
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
    for(auto zombie : zombies)
    {
        zombie->updateEntity();
    }

    for(auto plant : plants)
    {
        plant->updateEntity();
    }
}

void GameEngine::start()
{
    gameLoopTimer->start(GameConfig::GameLoopInterval);
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