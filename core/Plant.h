#pragma once
#include "GameEntity.h"
#include <QString>
#include <QElapsedTimer>
#include <QList>

class Zombie;
class Bullet;
class Sun;

// 植物基类
class Plant : public GameEntity
{
protected:
    int cost;
    QString plantType;

public:
    Plant(int r, int c, int hp_, int cost_, const QString& type);
    virtual ~Plant();

    QString getPlantType() const { return plantType; }
    int getCost() const { return cost; }
    int biteCount = 0;

    // 攻击（默认不可攻击）
    virtual bool canAttack(const QList<Zombie*>& zombies) const;
    virtual bool readyToAttack() const;
    virtual Bullet* createBullet();
    // 生产阳光（默认不可生产）
    virtual bool readyToProduceSun() const;
    virtual Sun* createSun();
};

// 豌豆射手
class Firefan : public Plant
{
    int attackDamage;
    int attackInterval;
    QElapsedTimer attackTimer;

public:
    Firefan(int r, int c);
    bool canAttack(const QList<Zombie*>& zombies) const override;
    bool readyToAttack() const override;
    Bullet* createBullet() override;
};

// 坚果墙
class Bengbear : public Plant
{
public:
    Bengbear(int r, int c);
};

// 向日葵
class Kimsunflower : public Plant
{
    int produceInterval;
    QElapsedTimer produceTimer;

public:
    Kimsunflower(int r, int c);
    bool readyToProduceSun() const override;
    Sun* createSun() override;
};

// 火爆辣椒（种下后蓄力，然后引爆同行全部僵尸后自毁）
class Rainchili : public Plant
{
    QElapsedTimer fuseTimer;

public:
    Rainchili(int r, int c);
    void startFuse();
    bool readyToExplode() const;
};
