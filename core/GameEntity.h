#pragma once
#include <QObject>
#include <QGraphicsPixmapItem>

enum class EntityType
{
    Plant,
    Zombie,
    Bullet,
    Sun,
    Effect
};

class GameEntity: public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
protected:
    int row, col, hp, maxHp;
    bool alive;
    bool deathAnimDone;
    EntityType entityType;
public:
    GameEntity(int r, int c, int hp_, EntityType entityType_);
    virtual ~GameEntity();
    virtual void updateEntity();
    virtual void takeDamage(int damage); // Entity受伤并判定是否死亡
    virtual void die();
    bool isAlive() const;                // 返回Entity存活情况
    int getHp() const;
    EntityType getEntityType() const;
    int getRow() const { return row; }
    int getCol() const { return col; }
    void markDeathAnimDone() { deathAnimDone = true; }
    bool isDeathAnimDone() const { return deathAnimDone; }
};