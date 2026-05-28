#include "Zombie.h"
#include "Plant.h"

Zombie::Zombie(int r, int c, int hp_, int speed_)
    : GameEntity(r, c, hp_, EntityType::Zombie)
    , speed(speed_)
    , attackDamage(20)
    , attackInterval(1000)
    , attacking(false)
    , targetPlant(nullptr)
{
    attackTimer.start();
}

Zombie::~Zombie() {}

void Zombie::moveLeft()
{
    moveBy(-speed, 0);
}

void Zombie::updateEntity()
{
    if (!attacking)
        moveLeft();
}

GenziZombie::GenziZombie(int row_, int col_): Zombie(row_, col_, 300, 1) {}

GenziZombie::~GenziZombie() {}

DancingZombie::DancingZombie(int row_, int col_): Zombie(row_, col_, 200, 2)
{
    spawnTimer.start();
}

DancingZombie::~DancingZombie() {}

void Zombie::startAttack(Plant* plant)
{
    attacking = true;
    targetPlant = plant;
    attackTimer.start();
}

void Zombie::stopAttack()
{
    attacking = false;
    targetPlant = nullptr;
}

bool Zombie::isAttacking() const { return attacking; }

Plant* Zombie::getTargetPlant() const { return targetPlant; }

bool Zombie::reachedHome() const
{
    return pos().x() <= 0;
}

bool Zombie::readyToDealDamage() const
{
    return attackTimer.elapsed() >= attackInterval;
}

void Zombie::resetAttackTimer()
{
    attackTimer.start();
}

int Zombie::getAttackDamage() const { return attackDamage; }

bool DancingZombie::readyToSpawn() const
{
    return spawnTimer.elapsed() >= spawnInterval;
}

void DancingZombie::resetSpawnTimer() { spawnTimer.start(); }

DancerZombie::DancerZombie(int row_, int col_) : Zombie(row_, col_, 100, 2) {}

DancerZombie::~DancerZombie() {}