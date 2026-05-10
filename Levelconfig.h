
#pragma once

struct LevelConfig
{
    int   level;
    int   enemiesToAdvance; 
    float spawnInterval;
    float enemyHP;
    float enemySpeed;
    float enemyDamage;
    int   enemyReward;
};

static const LevelConfig LEVEL1 =
{
    1,    
    15,    
    2.0f,   
    100.f,  
    80.f,   
    8.f,   
    10
};

static const LevelConfig LEVEL2 =
{
    2,
    20,    
    1.2f,
    180.f,
    130.f,
    15.f,
    20
};
