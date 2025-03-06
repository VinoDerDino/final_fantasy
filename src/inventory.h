#ifndef INVENTORY_H_
#define INVENTORY_H_

#define TARGET_EXTENSION 1

#include "pd_api.h"

#include "item.h"

#define MAX_ITEMS_IN_INVENTORY 50

typedef struct Inventory_S {
    Item items[MAX_ITEMS_IN_INVENTORY];
    int count;
    int curr_pos;
} Inventory;

#endif