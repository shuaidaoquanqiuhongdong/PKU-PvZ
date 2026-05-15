#include "Zombie.h"

Zombie::Zombie(int r, int c, int hp_, int speed_) : GameEntity(r, c, hp_, EntityType::Zombie), speed(speed_) {}

Zombie::~Zombie() {}

void Zombie::moveLeft()
{
    moveBy(-speed, 0);
}

void Zombie::updateEntity()
{
    moveLeft();
}

GenziZombie::GenziZombie(int row_, int col_): Zombie(row_, col_, 300, 1) {}

GenziZombie::~GenziZombie() {}