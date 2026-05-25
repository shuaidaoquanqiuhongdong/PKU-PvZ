#pragma once
#include "GameEntity.h"
#include <QElapsedTimer>

class Plant;

class Zombie : public GameEntity
{
protected:
    int speed; // 僵尸的移动速度
    int attackDamage;
    int attackInterval;
    bool attacking;
    Plant* targetPlant;
    QElapsedTimer attackTimer;
public:
    Zombie(int r, int c, int hp_, int speed_);
    ~Zombie();
    void updateEntity() override;
    void moveLeft();
    void startAttack(Plant* plant);
    void stopAttack();
    bool isAttacking() const;
    Plant* getTargetPlant() const;
    bool reachedHome() const;
    bool readyToDealDamage() const;
    void resetAttackTimer();
    int getAttackDamage() const;
};

class GenziZombie : public Zombie
{
public:
    GenziZombie(int row_, int col_);
    ~GenziZombie();
};

class DancingZombie : public Zombie
{
public:
    DancingZombie(int row_, int col_);
    ~DancingZombie();
};