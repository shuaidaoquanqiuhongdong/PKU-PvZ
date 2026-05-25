#include "Plant.h"
#include "Zombie.h"
#include "Bullet.h"

Plant::Plant(int r, int c, int hp_, int cost_, const QString& type) : GameEntity(r,c,hp_,EntityType::Plant), cost(cost_), plantType(type), attackDamage(20), attackInterval(1500), canShoot(true)
{
    attackTimer.start();
}

Plant::~Plant() {}

void Plant::updateEntity()
{

}

bool Plant::canAttack(const QList<Zombie*>& zombies) const
{
    for(auto* z : zombies)
    {
        if (z->isAlive() && z->getRow() == row)
            return true;
    }
    return false;
}

bool Plant::readyToAttack()const
{
    return attackTimer.elapsed() >= attackInterval;
}

Bullet* Plant::createBullet()
{
    attackTimer.start(); // 重置冷却
    Bullet* b = new Bullet(row, col, attackDamage, 5); // speed=5
    // 子弹出生位置：植物右侧
    b->setPos(pos() + QPointF(35, -10));
    return b;
}


