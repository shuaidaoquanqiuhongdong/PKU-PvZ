#include "Plant.h"

Plant::Plant(int r, int c, int hp_, int cost_) : GameEntity(r, c, hp_, EntityType::Plant), cost(cost_) {}

Plant::~Plant() {}

void Plant::updateEntity()
{

}