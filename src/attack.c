#include "attack.h"

Attack knight_attacks[] = {
    {"Stab", 5, BASIC_ATTACK, SINGLE_TARGET},
    {"Swing", 5, BASIC_ATTACK, COLUMN},
    {"Plunge", 5, BASIC_ATTACK, SINGLE_TARGET},
    {"Charge", 5, BASIC_ATTACK, ROW},
};

Attack mage_attacks[] = {
    {"Fireball", 5, FIRE_ATTACK, SINGLE_TARGET},
    {"Waterjet", 5, WATER_ATTACK, ROW},
    {"Meteor", 5, BASIC_ATTACK, AREA_ROW_COL},
    {"Heal", 5, BUFF_HEAL, SINGLE_TARGET},
};

Attack hunter_attacks[] = {
    {"Shot", 5, BASIC_ATTACK, SINGLE_TARGET},
    {"Sharpshoot", 5, PIERCING_ATTACK, COLUMN},
    {"Threeshot", 5, BASIC_ATTACK, COLUMN},
    {"Weakening", 5, DEBUFF_ARMOR, SINGLE_TARGET},
};
