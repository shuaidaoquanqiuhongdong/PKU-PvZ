#pragma once
#include <QString>

namespace GameConfig
{
    inline constexpr int SceneWidth = 1408;
    inline constexpr int SceneHeight = 768;
    inline constexpr int Rows = 5;
    inline constexpr int Cols = 9;
    inline constexpr int GridStartX = 270;
    inline constexpr int GridStartY = 140;
    inline constexpr int CellWidth = 106;
    inline constexpr int CellHeight = 124;

    inline constexpr int InitialSun = 150;

    inline constexpr int GameLoopInterval = 33;
    inline constexpr int ZombieSpawnInterval = 10000;
    inline constexpr int SunGenerateInterval = 11000;

    inline constexpr int FirefanCost = 100;
    inline constexpr int FirefanHp = 300;
    inline constexpr int FirefanDamage = 20;
    inline constexpr int FirefanAttackInterval = 1500;

    inline constexpr int BengbearCost = 50;
    inline constexpr int BengbearHp = 800;

    inline constexpr int KimsunflowerCost = 50;
    inline constexpr int KimsunflowerHp = 200;
    inline constexpr int KimsunflowerProduceInterval = 12000;

    inline constexpr int RainchiliCost = 125;
    inline constexpr int RainchiliHp = 1;
    inline constexpr int RainchiliFuseTime = 1500;

    inline constexpr int ZombieAttackDamage = 20;
    inline constexpr int ZombieAttackInterval = 1000;

    inline constexpr int GenziZombieCost = 50;
    inline constexpr int GenziZombieHp = 300;
    inline constexpr int GenziZombieSpeed = 1;

    inline constexpr int DancingZombieCost = 100;
    inline constexpr int DancingZombieHp = 200;
    inline constexpr int DancingZombieSpeed = 1;
    inline constexpr int DancingZombieSpawnInterval = 8000;

    inline constexpr int DancerZombieHp = 100;
    inline constexpr int DancerZombieSpeed = 1;

    inline constexpr int BulletSpeed = 5;

    inline constexpr int FlowerSunValue = 25;
    inline constexpr int SkySunValue = 25;

    inline int getPlantCost(const QString& type)
    {
        if (type == "Firefan")       return FirefanCost;
        if (type == "Bengbear")      return BengbearCost;
        if (type == "Kimsunflower")  return KimsunflowerCost;
        if (type == "Rainchili")     return RainchiliCost;
        return 0;
    }

    inline int getPlantHp(const QString& type)
    {
        if (type == "Firefan")       return FirefanHp;
        if (type == "Bengbear")      return BengbearHp;
        if (type == "Kimsunflower")  return KimsunflowerHp;
        if (type == "Rainchili")     return RainchiliHp;
        return 300;
    }

    inline int getZombieCost(const QString& type)
    {
        if (type == "GenziZombie")   return GenziZombieCost;
        if (type == "DancingZombie") return DancingZombieCost;
        return 0;
    }

    inline int getZombieHp(const QString& type)
    {
        if (type == "GenziZombie")   return GenziZombieHp;
        if (type == "DancingZombie") return DancingZombieHp;
        if (type == "DancerZombie")  return DancerZombieHp;
        return 200;
    }

    inline int getZombieSpeed(const QString& type)
    {
        if (type == "GenziZombie")   return GenziZombieSpeed;
        if (type == "DancingZombie") return DancingZombieSpeed;
        if (type == "DancerZombie")  return DancerZombieSpeed;
        return 1;
    }
}
