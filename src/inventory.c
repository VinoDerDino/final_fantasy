#include "inventory.h"

void invOnEnter(void* params) {
    InventoryParams* invParams = (InventoryParams*) params;
    PlaydateAPI* pd = invParams->pd;
    
    for(int i = 0; i < 4; i++) {
        pd->graphics->drawRect(56, 14 + i * 50, 330, 30, kColorBlack);
        pd->graphics->drawLine(150, 14 + i * 50, 150, 42 + i * 50, 1, kColorBlack);
    }
}

void invOnExit(void* params) {

}

void invUpdate(void* params) {
    InventoryParams* invParams = (InventoryParams*) params;
    PlaydateAPI* pd = invParams->pd;
    Inventory* inv = invParams->inv;

    float crankChange = pd->system->getCrankChange();

    if(crankChange <= -5) {
        if(inv->curr_pos - 1 >= 0) {
            inv->curr_pos--;
            pd->system->logToConsole("New pos: %d", inv->curr_pos);
        }
    } else if(crankChange >= 5) {
        if(inv->curr_pos + 1 < inv->count) {
            inv->curr_pos++;
            pd->system->logToConsole("New pos: %d", inv->curr_pos);
        }
    }
}

void drawText(Item item, PlaydateAPI* pd) {
    char attackStr[10], critChanceStr[10], valueStr[10];
    snprintf(attackStr, sizeof(attackStr), "%d", item.attack);
    snprintf(critChanceStr, sizeof(critChanceStr), "%d", item.crit_chance);
    snprintf(valueStr, sizeof(valueStr), "%d", item.value);

    pd->graphics->drawText("Name", strlen("Name"), kASCIIEncoding, 60, 20);
    pd->graphics->drawText(item.name, strlen(item.name), kASCIIEncoding, 160, 20);
    pd->graphics->drawText("Attack", strlen("Attack"), kASCIIEncoding, 60, 70);
    pd->graphics->drawText(attackStr, strlen(attackStr), kASCIIEncoding, 160, 70);
    pd->graphics->drawText("Crit chance", strlen("Crit chance"), kASCIIEncoding, 60, 120);
    pd->graphics->drawText(critChanceStr, strlen(critChanceStr), kASCIIEncoding, 160, 120);
    pd->graphics->drawText("Value", strlen("Value"), kASCIIEncoding, 60, 170);
    pd->graphics->drawText(valueStr, strlen(valueStr), kASCIIEncoding, 160, 170);
}

void invDraw(void* params) {
    InventoryParams* invParams = (InventoryParams*) params;
    PlaydateAPI* pd = invParams->pd;
    Inventory* inv = invParams->inv;
    invParams->selectedItem = inv->curr_pos;

    pd->graphics->clear(kColorWhite);
    pd->graphics->drawRect(9, 3, 33, 33, kColorBlack); 

    int visibleIndex = 0;
    for(int i = inv->curr_pos; i < inv->curr_pos + 7 && i < inv->count; i++) {
        drawSprite(inv->items[i].icon, pd, 10, 3 + visibleIndex * 33, 2.0);
        visibleIndex++;
    }

    drawText(inv->items[inv->curr_pos], pd);
}
