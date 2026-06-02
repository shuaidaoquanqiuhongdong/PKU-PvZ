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
    spawnTimer = new QTimer(this);
    spawnTimer->setSingleShot(false);

    // 随机初始延迟 3~7s，让不同舞王僵尸的生成节奏错开
    int initialDelay = 3000 + rand() % 4000;
    QTimer::singleShot(initialDelay, this, [this]() {
        if (isAlive() && !isAttacking())
            emit readyToSpawn(this);
        spawnTimer->start(GameConfig::DancingZombieSpawnInterval);
    });

    connect(spawnTimer, &QTimer::timeout, this, [this]() {
        if (isAlive() && !isAttacking())
            emit readyToSpawn(this);
    });
}

DancingZombie::~DancingZombie()
{
    spawnTimer->stop();
}

void DancingZombie::stopSpawnTimer()
{
    spawnTimer->stop();
}

void DancingZombie::startSpawnTimer()
{
    if (!spawnTimer->isActive())
        spawnTimer->start();
}

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

DancerZombie::DancerZombie(int row_, int col_) : Zombie(row_, col_, GameConfig::DancerZombieHp, GameConfig::DancerZombieSpeed) {}

DancerZombie::~DancerZombie() {}