#include "inventory.h"

void invOnEnter(void* params) {
    InventoryParams* invParams = (InventoryParams*) params;
    PlaydateAPI* pd = invParams->pd;

    pd->graphics->clear(kColorWhite);
    pd->system->logToConsole("New Scene Inventory");
}

void invOnExit(void* params) {

}

void invUpdate(void* params, float dt) {
    InventoryParams* invParams = (InventoryParams*) params;
    PlaydateAPI* pd = invParams->pd;
    Inventory* inv = invParams->inv;

    float crankChange = pd->system->getCrankChange() * 0.15f;
    PDButtons btn;
    pd->system->getButtonState(NULL, &btn, NULL);

    if(btn & kButtonUp) {
        inv->curr_pos -= 1;
        if(inv->curr_pos < 0) inv->curr_pos = 0;
    } else if(btn  & kButtonDown) {
        inv->curr_pos += 1;
        if(inv->curr_pos >= inv->count) inv->curr_pos = inv->count - 1;
    }

    if(crankChange != 0) {
        pd->system->logToConsole("Crank change: %f", crankChange);
        inv->curr_pos += (int)round(crankChange);
        if(inv->curr_pos < 0) {
            inv->curr_pos = 0;
        } else if(inv->curr_pos >= inv->count) {
            inv->curr_pos = inv->count - 1;
        }
        pd->system->logToConsole("New inv pos: %d", inv->curr_pos);
    }

    // invOnEnter(params); // Removed to prevent unnecessary UI redraw
    invDraw(params);
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

    pd->graphics->clear(kColorWhite);
    pd->graphics->drawRect(9, 3, 33, 33, kColorBlack); 

    for(int i = 0; i < 4; i++) {
        pd->graphics->drawRect(56, 13 + i * 50, 330, 30, kColorBlack);
        pd->graphics->drawLine(150, 13 + i * 50, 150, 42 + i * 50, 1, kColorBlack);
    }

    int visibleIndex = 0;
    for(int i = (int)inv->curr_pos; i < (int)inv->curr_pos + 7 && i < inv->count; i++) { //7 statt 3
        drawSprite(inv->items[i].icon, pd, 10, 3 + visibleIndex * 33, kBitmapUnflipped); //33 statt 65
        visibleIndex++;
    }

    drawText(inv->items[(int)inv->curr_pos], pd);
} 
