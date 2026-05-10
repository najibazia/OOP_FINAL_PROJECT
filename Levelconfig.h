// #pragma once

// // ── Level configuration ───────────────────────────────────────────────────────
// // All level settings live here — easy to tweak without touching Game.cpp

// struct LevelConfig
// {
//     int   level;             // level number (1, 2, ...)
//     int   enemiesToAdvance;  // kill this many to go to next level
//     float spawnInterval;     // seconds between enemy spawns
//     float enemyHP;           // enemy starting HP
//     float enemySpeed;        // enemy movement speed (pixels/sec)
//     float enemyDamage;       // damage per second to tower when attacking
//     int   enemyReward;       // gold per kill (unused for now)
// };

// // ── Level 1 ───────────────────────────────────────────────────────────────────
// // Straight path, slow enemies, relaxed spawn rate
// static const LevelConfig LEVEL1 =
// {
//     1,      // level
//     10,     // kill 10 enemies to advance
//     2.0f,   // spawn every 2 seconds
//     100.f,  // 100 HP
//     80.f,   // 80 px/sec
//     8.f,    // 8 damage/sec to tower
//     10      // 10 gold
// };

// // ── Level 2 ───────────────────────────────────────────────────────────────────
// // Winding path, tougher enemies, faster spawns
// static const LevelConfig LEVEL2 =
// {
//     2,      // level
//     20,     // kill 20 enemies to finish
//     1.2f,   // spawn every 1.2 seconds (faster!)
//     180.f,  // 180 HP (almost double)
//     130.f,  // 130 px/sec (much faster)
//     15.f,   // 15 damage/sec to tower
//     20      // 20 gold
// };

#pragma once

struct LevelConfig
{
    int   level;
    int   enemiesToAdvance;  // kill this many to go to next level
    float spawnInterval;
    float enemyHP;
    float enemySpeed;
    float enemyDamage;
    int   enemyReward;
};

static const LevelConfig LEVEL1 =
{
    1,      // level
    15,     // ← 15 kills needed to advance
    2.0f,   // spawn every 2 seconds
    100.f,  // 100 HP
    80.f,   // 80 px/sec
    8.f,    // 8 damage/sec to tower
    10
};

static const LevelConfig LEVEL2 =
{
    2,
    20,     // ← 20 kills to win
    1.2f,
    180.f,
    130.f,
    15.f,
    20
};