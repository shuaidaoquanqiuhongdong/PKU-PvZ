#pragma once

class Entity
{
protected:
    int row, col, hp;
    bool dead;
public:
    Entity(int r, int c, int hp_);
    virtual ~Entity();
    virtual void takeDamage(int damage); // Entity受伤并判定是否死亡
    bool isDead() const;                 // 返回Entity存活情况
    virtual void tick();
};