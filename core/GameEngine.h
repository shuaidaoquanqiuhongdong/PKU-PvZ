#pragma once
#include "GridManager.h"
#include "Zombie.h"
#include <QList>
#include <QTimer>
#include "Bullet.h"
#include "Sun.h"
#include "../animation/AnimationManager.h"

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
    QTimer* zombieSpawnTimer;
    int spawnedZombieCount;
    int maxZombieCount;
    bool running;
    bool zombieMode = false;
    int currentWave;
    int totalWave;
    int zombiesInCurrentWave;
    bool waveCleared;
    QHash<QString, QElapsedTimer> plantCooldowns;

public:
    GameEngine();
    void addPlant(Plant* plant);
    void addZombie(Zombie* zombie);
    void updateGame();
    void start();
    void startZombieMode();
    void pause();
    void resume();
    void stop();
    bool placePlant(QString plantType, int row, int col);
    bool removePlant(int row, int col);
    bool shovelPlant(int row, int col);
    bool placeZombie(QString zombieType, int row, int col);
    void checkPlantAttack();
    void checkSunProduction();
    void checkRainchiliFuse();
    void onDancingZombieSpawn(DancingZombie* dancing);
    void updateBullets();
    void checkCollisions();
    void cleanupDeadEntities();
    void generateSun();
    void spawnZombie();
    void collectSun(Sun* sun);
    void checkZombieAttackPlant();
    void checkGameResult();
    void checkWaveTransition();
    void startNextWave();
    void removeEntitySafely(GameEntity* entity);
    int getPlantCooldownRemaining(const QString& type) const;
    bool isZombieMode() const { return zombieMode; }
signals:
    void sunChanged(int value);
    void waveChanged(int currentWave, int totalWave);
    void entityCreated(GameEntity* entity);
    void entityDied(GameEntity* entity);
    void gameOver(bool win);
    void bulletHit(QPointF pos);
    void sunCreated(Sun* sun);
    void sunCollected(Sun* sun);
    void entityAnimationChanged(GameEntity* entity, AnimationState state);
    void plantCooldownChanged(const QString& plantType, int cooldownRemainingMs);
};
