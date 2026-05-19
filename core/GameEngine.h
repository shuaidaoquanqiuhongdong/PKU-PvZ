#pragma once
#include "GridManager.h"
#include "Zombie.h"
#include <QList>
#include <QTimer>
#include "Bullet.h"
#include "Sun.h"

class GameEngine: public QObject
{
    Q_OBJECT
private:
    QList<Plant*> plants;
    QList<Zombie*> zombies;
    GridManager* gridManager;
    QTimer* gameLoopTimer;
    QList<Bullet*> bullets;
    int sunValue;
    QList<Sun*> suns;
    QTimer* sunGenerateTimer;
public:
    GameEngine();
    void addPlant(Plant* plant);
    void addZombie(Zombie* zombie);
    void updateGame();
    void start();
    bool placePlant(QString plantType, int row, int col);
    void checkPlantAttack();
    void updateBullets();
    void checkCollisions();
    void cleanupDeadEntities();
    void generateSun();
    void collectSun(Sun* sun);
signals:
    void sunChanged(int value);               // 阳光变化，前端更新显示
    void entityCreated(GameEntity* entity);   // 新实体诞生，前端加进场景
    void entityDied(GameEntity* entity);      // 实体死亡，动画播放死亡效果
    void gameOver(bool win);                  // 游戏结束，前端切换页面
    void bulletHit(QPointF pos);
    void sunCreated(Sun*sun);
    void sunCollected(Sun* sun);
};