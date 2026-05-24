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
    if (attacking && targetPlant)
    {
        if (attackTimer.elapsed() >= attackInterval)
        {
            targetPlant->takeDamage(attackDamage);
            attackTimer.start();
            if (!targetPlant->isAlive())
            {
                stopAttack();
            }
        }
    }
    else
    {
        moveLeft();
    }
}

GenziZombie::GenziZombie(int row_, int col_): Zombie(row_, col_, 300, 1) {}

GenziZombie::~GenziZombie() {}

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