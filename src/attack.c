#include "attack.h"

Attack knight_attacks[] = {
    {"Stab", 5, 0, 0, 0.0, BASIC_ATTACK, SINGLE_TARGET},
    {"Swing", 5, 0, 0, 0.0, BASIC_ATTACK, AREA_COL},
    {"Plunge", 5, 0, 0, 0.0, BASIC_ATTACK, SINGLE_TARGET},
    {"Charge", 5, 0, 0, 0.0, BASIC_ATTACK, ROW},
};

Attack mage_attacks[] = {
    {"Fireball", 5, 0, 0, 0.0, FIRE_ATTACK, SINGLE_TARGET},
    {"Waterjet", 5, 0, 0, 0.0, WATER_ATTACK, AREA_COL},
    {"Meteor", 5, 0, 0, 0.0, BASIC_ATTACK, AREA_ROW_COL},
    {"Heal", 5, 0, 0, 0.0, BUFF_HEAL, SINGLE_TARGET},
};

Attack hunter_attacks[] = {
    {"Shot", 5, 0, 0, 0.0, BASIC_ATTACK, SINGLE_TARGET},
    {"Sharpshoot", 5, 0, 0, 0.0, PIERCING_ATTACK, AREA_COL},
    {"Threeshot", 5, 0, 0, 0.0, BASIC_ATTACK, COLUMN},
    {"Weakening", 5, 0, 0, 0.0, DEBUFF_ARMOR, SINGLE_TARGET},
};
