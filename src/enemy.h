#ifndef ENEMY_H_
#define ENEMY_H_

#include "sprite.h"
#include "attack.h"

typedef enum {
    STANDARD,
    FIRE,
    POISON
} AttackTypeEnemy;

typedef struct {
    const char *name;
    int size_col, size_row;
    AttackTypeEnemy type;
} AttackEnemy;

typedef struct {
    const char* name;
    int id;
    int hp;
    int attack;
    int defense;
    int resistence;
    int lvl;
    int attackCount;
    AttackEnemy* attacks;
    int fight_x, fight_y;
    AttackType resistenceType;
} Enemy;

extern Enemy enemies[];

#endif