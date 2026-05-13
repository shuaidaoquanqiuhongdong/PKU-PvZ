#include "GameEntity.h"

Entity::Entity(int r, int c, int hp_) : row(r), col(c), hp(hp_), dead(false) {}

Entity::~Entity() {}

void Entity::takeDamage(int damage)
{
    hp -= damage;
    if (hp <= 0)
    {
        dead = true;
    }
}

bool Entity::isDead() const
{
    return dead;
}

void Entity::tick() {}