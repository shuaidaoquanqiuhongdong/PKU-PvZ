#include "Bullet.h"
#include "Zombie.h"
#include "GameConfig.h"

Bullet::Bullet(int r, int c, int damage_, int speed_) : GameEntity(r, c, 1, EntityType::Bullet), damage(damage_), speed(speed_) {}

Bullet::~Bullet() {}

void Bullet::updateEntity()
{
    moveBy(speed, 0);
    if (pos().x() > GameConfig::SceneWidth)
        alive = false;
}

bool Bullet::collideWithZombie(Zombie* zombie)
{
    if(!zombie || !zombie->isAlive())
        return false;
    if(zombie->getRow() != row)
        return false;
    qreal dx = zombie->pos().x() - pos().x();
    return(dx > 0 && dx < 30);
}