#pragma once

namespace GameConfig
{
    // --- 场景与网格设置 ---(等后续背景制作出来再修改参数)
    inline constexpr int SceneWidth = 900;  // 场景总宽度
    inline constexpr int SceneHeight = 550; // 场景总高度
    inline constexpr int Rows = 5;          // 草坪行数
    inline constexpr int Cols = 9;          // 草坪列数
    inline constexpr int GridStartX = 120;  // 网格起始X坐标（给左边留出界面空间）
    inline constexpr int GridStartY = 10;   // 网格起始Y坐标（给顶部留出界面空间）
    inline constexpr int CellWidth = 80;    // 每个格子的宽度
    inline constexpr int CellHeight = 90;   // 每个格子的高度

    // --- 游戏资源设置 ---
    inline constexpr int InitialSun = 150; // 开局初始阳光值

    // --- 定时器间隔设置 (毫秒) ---
    inline constexpr int GameLoopInterval = 33;      // 游戏主循环刷新时间 (约30帧/秒)
    inline constexpr int ZombieSpawnInterval = 6000; // 刷僵尸的间隔 (6秒)
    inline constexpr int SunGenerateInterval = 8000; // 刷系统阳光的间隔 (8秒)

    // === 你可以在这里继续扩充你们的常量 ===
}