#ifndef ENEMY_H_
#define ENEMY_H_

typedef enum {
    STANDARD,
    FIRE,
    POISON
} AttackType;

typedef struct {
    const char *name;
    int size_col, size_row;
    AttackType type;
} Attack;

typedef struct {
    const char* name;
    int hp;
    int attack;
    int defense;
    int resistence;
    int lvl;
    int attackCount;
    Attack* attacks;
} Enemy;


extern Enemy enemies[];

int dealDamage(Enemy e);
void recieveDamage(Enemy* e, int dmg);


#endif