#include "Zombie.h"
#include "Plant.h"
#include "GameConfig.h"

Zombie::Zombie(int r, int c, int hp_, int speed_)
    : GameEntity(r, c, hp_, EntityType::Zombie)
    , speed(speed_)
    , attackDamage(GameConfig::ZombieAttackDamage)
    , attackInterval(GameConfig::ZombieAttackInterval)
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

GenziZombie::GenziZombie(int row_, int col_): Zombie(row_, col_, GameConfig::GenziZombieHp, GameConfig::GenziZombieSpeed) {}

GenziZombie::~GenziZombie() {}

DancingZombie::DancingZombie(int row_, int col_): Zombie(row_, col_, GameConfig::DancingZombieHp, GameConfig::DancingZombieSpeed)
{
    spawnInterval = GameConfig::DancingZombieSpawnInterval;
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

DancerZombie::DancerZombie(int row_, int col_) : Zombie(row_, col_, GameConfig::DancerZombieHp, GameConfig::DancerZombieSpeed) {}

DancerZombie::~DancerZombie() {}