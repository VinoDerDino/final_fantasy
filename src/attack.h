#ifndef ATTACK_H_
#define ATTACK_H_

typedef enum {
    SINGLE_TARGET,
    ROW,
    COLUMN,
    AREA_ROW,
    AREA_COL,
    AREA_ROW_COL,
} AttackPattern;

typedef enum {
    BASIC_ATTACK,
    PIERCING_ATTACK,
    FIRE_ATTACK,
    WATER_ATTACK,
    GROUND_ATTACK,
    BUFF_HEAL,
    DEBUFF_ARMOR
} AttackType;

typedef struct {
    const char* name;
    int dmg;
    int rect_x, dest_y;
    float rect_y;
    AttackType type;
    AttackPattern pattern;
} Attack;

extern Attack knight_attacks[];
extern Attack mage_attacks[];
extern Attack hunter_attacks[];

#endif