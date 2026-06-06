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

DancingZombie::DancingZombie(int row_, int col_)
    : Zombie(row_, col_,
             GameConfig::DancingZombieHp,
             GameConfig::DancingZombieSpeed)
    , spawnTimer(new QTimer(this))
    , summoning(false)
{
    spawnTimer->setSingleShot(false);

    connect(spawnTimer, &QTimer::timeout,
            this, &DancingZombie::beginSummon);

    const int initialDelay = 3000 + rand() % 4000;

    QTimer::singleShot(initialDelay, this, [this]() {
        if (!isAlive()) return;

        beginSummon();
        spawnTimer->start(GameConfig::DancingZombieSpawnInterval);
    });
}

DancingZombie::~DancingZombie()
{
    spawnTimer->stop();
}

void DancingZombie::updateEntity()
{
    if (summoning) {
        return;
    }

    Zombie::updateEntity();
}

void DancingZombie::beginSummon()
{
    if (!isAlive() || summoning) {
        return;
    }

    stopAttack();
    summoning = true;
    emit summonAnimationStarted(this);

    QTimer::singleShot(
        GameConfig::DancingZombieDanceDuration,
        this,
        [this]() {
            if (!isAlive()) {
                summoning = false;
                return;
            }

            summoning = false;
            emit readyToSpawn(this);
        }
    );
}

void DancingZombie::stopSpawnTimer()
{
    spawnTimer->stop();
}

void DancingZombie::startSpawnTimer()
{
    if (!spawnTimer->isActive()) {
        spawnTimer->start(GameConfig::DancingZombieSpawnInterval);
    }
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