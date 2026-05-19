#include "GameEntity.h"

GameEntity::GameEntity(int r, int c, int hp_, EntityType entityType_) : row(r), col(c), hp(hp_), maxHp(hp_), alive(true), entityType(entityType_) {}

GameEntity::~GameEntity() {}

//待开发
void GameEntity::updateEntity()
{

}

void GameEntity::takeDamage(int damage)
{
    if (damage <= 0)
    {
        return;
    }
    hp -= damage;
    if (hp <= 0)
    {
        die();
    }
}

void GameEntity::die()
{
    if (!alive)
    {
        return;
    }
    alive = false;
}

bool GameEntity::isAlive() const
{
    return alive;
}

int GameEntity::getHp() const
{
    return hp;
}

EntityType GameEntity::getEntityType() const
{
    return entityType;
}