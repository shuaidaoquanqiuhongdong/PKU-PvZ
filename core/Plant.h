#pragma once
#include "GameEntity.h"
#include <QString>
#include <QElapsedTimer>

class Plant : public GameEntity
{
protected:
    int cost; // 植物消耗的阳光数量
    QString plantType;
    int attackDamage;
    int attackInterval;
    bool canShoot;
    QElapsedTimer attackTimer;
public:
    Plant(int r, int c, int hp_, int cost_, const QString& type);
    ~Plant();
    void updateEntity() override;
    bool canAttack(const QList<class Zombie*>& zombies) const;
    bool readyToAttack() const;
    class Bullet* createBullet();
<<<<<<< HEAD

    QString getPlantType() const;
=======
    QString getPlantType() const { return plantType; }
    int getCost() const { return cost; }
>>>>>>> 46c5996a837aeaa02c1cafaddee249428e9d3f33
};