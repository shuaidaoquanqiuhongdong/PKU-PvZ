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