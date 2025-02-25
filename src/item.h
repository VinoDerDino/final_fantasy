#ifndef ITEM_H_
#define ITEM_H_

#include "pd_api.h"
#include "sprite.h"
#include <stdint.h>

#define MAX_ITEMS 30
#define MAX_NAME_LEN 20
#define MAX_DESC_LEN 50

typedef enum {
    ITEM_TYPE_POTION,
    ITEM_TYPE_WEAPON,
    ITEM_TYPE_ARMOR,
    ITEM_TYPE_ACCESSORY,
    ITEM_TYPE_MISC
} ItemType;

typedef enum {
    EFFECT_NONE,
    EFFECT_DAMAGE_BOOST,
    EFFECT_SET_BONUS,
    EFFECT_LIFESTEAL,
    EFFECT_CRIT_BOOST,
    EFFECT_DEFENSE_STACK,
} ItemEffect;

typedef struct {
    uint8_t id;
    char name[MAX_NAME_LEN];
    char description[MAX_DESC_LEN];
    ItemType type;
    
    union {
        struct {
            uint8_t attack;
            uint8_t crit_chance;
            uint8_t hit_counter;
        };
        struct {
            uint8_t defense;
            uint8_t set_id;
        };
        struct {
            uint8_t heal_value;
        };
    };

    ItemEffect effect;
    uint8_t effect_value;
    uint8_t value;
    Sprite icon;
} Item;

typedef struct {
    Item items[MAX_ITEMS];
    int current_index;
} ItemList;

#endif