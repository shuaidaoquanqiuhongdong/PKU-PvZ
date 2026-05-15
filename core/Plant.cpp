#include "Plant.h"

Plant::Plant(int r, int c, int hp_, int cost_, QString planttype) : GameEntity(r, c, hp_, EntityType::Plant), cost(cost_), plantType(planttype) {}

Plant::~Plant() {}

void Plant::updateEntity()
{

}