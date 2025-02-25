#ifndef INVENTORY_H_
#define INVENTORY_H_

#define TARGET_EXTENSION 1

#include "pd_api.h"
#include "item.h"
#include "player.h"
#include "sprite.h"

#define MAX_ITEMS_IN_INVENTORY 50

typedef struct  {
    Item items[MAX_ITEMS_IN_INVENTORY];
    int count;
    int curr_pos;
} Inventory;

typedef struct {
    PlaydateAPI* pd;
    int selectedItem;
    Inventory* inv;
} InventoryParams;

void invOnEnter(void* params);
void invOnExit(void* params);
void invUpdate(void* params);
void invDraw(void* params);

#endif