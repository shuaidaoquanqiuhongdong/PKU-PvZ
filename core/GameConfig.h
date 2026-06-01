#pragma once
#include <QString>

namespace GameConfig
{
    // --- 场景与网格设置 ---
    inline constexpr int SceneWidth = 1408; // 场景总宽度
    inline constexpr int SceneHeight = 768; // 场景总高度
    inline constexpr int Rows = 5;          // 草坪行数
    inline constexpr int Cols = 9;          // 草坪列数
    inline constexpr int GridStartX = 270;  // 网格起始X坐标（给左边留出界面空间）
    inline constexpr int GridStartY = 140;  // 网格起始Y坐标（给顶部留出界面空间）
    inline constexpr int CellWidth = 106;   // 每个格子的宽度
    inline constexpr int CellHeight = 124;  // 每个格子的高度

    // --- 游戏资源设置 ---
    inline constexpr int InitialSun = 150; // 开局初始阳光值

    // --- 定时器间隔设置 (毫秒) ---
    inline constexpr int GameLoopInterval = 33;      // 游戏主循环刷新时间 (约30帧/秒)
    inline constexpr int ZombieSpawnInterval = 10000; // 刷僵尸的初始间隔 (10秒)
    inline constexpr int SunGenerateInterval = 11000; // 刷系统阳光的间隔 (11秒)

    // --- 植物属性 ---
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

    // --- 僵尸属性 ---
    inline constexpr int ZombieAttackDamage = 20;
    inline constexpr int ZombieAttackInterval = 1000;

    inline constexpr int GenziZombieCost = 50;
    inline constexpr int GenziZombieHp = 300;
    inline constexpr int GenziZombieSpeed = 1;

    inline constexpr int DancingZombieCost = 100;
    inline constexpr int DancingZombieHp = 200;
    inline constexpr int DancingZombieSpeed = 1;
    inline constexpr int DancingZombieSpawnInterval = 5000;

    inline constexpr int DancerZombieHp = 100;
    inline constexpr int DancerZombieSpeed = 1;

    // --- 子弹属性 ---
    inline constexpr int BulletSpeed = 5;

    // --- 阳光属性 ---
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
        if (type == "GenziZombie")       return GenziZombieCost;
        if (type == "DancingZombie")     return DancingZombieCost;
        return 50;
    }
}