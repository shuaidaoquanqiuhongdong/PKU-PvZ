#include "Plant.h"
#include "Zombie.h"
#include "Bullet.h"
#include "Sun.h"
#include "GameConfig.h"

// ========== Plant 基类 ==========

Plant::Plant(int r, int c, int hp_, int cost_, const QString& type)
    : GameEntity(r, c, hp_, EntityType::Plant)
    , cost(cost_)
    , plantType(type)
{}

Plant::~Plant() {}

bool Plant::canAttack(const QList<Zombie*>&) const { return false; }
bool Plant::readyToAttack() const { return false; }
Bullet* Plant::createBullet() { return nullptr; }
bool Plant::readyToProduceSun() const { return false; }
Sun* Plant::createSun() { return nullptr; }

// ========== Firefan（豌豆射手）==========

Firefan::Firefan(int r, int c)
    : Plant(r, c, GameConfig::FirefanHp, GameConfig::FirefanCost, "Firefan")
    , attackDamage(GameConfig::FirefanDamage)
    , attackInterval(GameConfig::FirefanAttackInterval)
{
    attackTimer.start();
}

bool Firefan::canAttack(const QList<Zombie*>& zombies) const
{
    for (auto* z : zombies) {
        if (z->isAlive() && z->getRow() == row)
            return true;
    }
    return false;
}

bool Firefan::readyToAttack() const
{
    return attackTimer.elapsed() >= attackInterval;
}

Bullet* Firefan::createBullet()
{
    attackTimer.start();
    Bullet* b = new Bullet(row, col, attackDamage, GameConfig::BulletSpeed);
    b->setPos(pos() + QPointF(35, -10));
    return b;
}

// ========== Bengbear（坚果墙）==========

Bengbear::Bengbear(int r, int c)
    : Plant(r, c, GameConfig::BengbearHp, GameConfig::BengbearCost, "Bengbear")
{}

// ========== Kimsunflower（向日葵）==========

Kimsunflower::Kimsunflower(int r, int c)
    : Plant(r, c, GameConfig::KimsunflowerHp, GameConfig::KimsunflowerCost, "Kimsunflower")
    , produceInterval(GameConfig::KimsunflowerProduceInterval)
{
    produceTimer.start();
}

bool Kimsunflower::readyToProduceSun() const
{
    return produceTimer.elapsed() >= produceInterval;
}

Sun* Kimsunflower::createSun()
{
    produceTimer.start();
    Sun* s = new Sun(GameConfig::FlowerSunValue);
    s->setPos(pos() + QPointF(0, -20));
    return s;
}

// ========== Rainchili（火爆辣椒）==========

Rainchili::Rainchili(int r, int c)
    : Plant(r, c, GameConfig::RainchiliHp, GameConfig::RainchiliCost, "Rainchili")
{}

void Rainchili::startFuse()
{
    fuseTimer.start();
}

bool Rainchili::readyToExplode() const
{
    return fuseTimer.isValid() && fuseTimer.elapsed() >= GameConfig::RainchiliFuseTime;
}
