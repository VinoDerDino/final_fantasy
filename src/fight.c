#include "fight.h"

void battleOnEnter(void *params) {
    BattleParams* battleParams = (BattleParams*)params;
    PlaydateAPI* pd = battleParams->pd;
    LCDBitmapTable* monsters = battleParams->monsters;

    
    pd->graphics->clear(kColorWhite);
    pd->system->logToConsole("New Scene Fight");

    int start_x = 14, start_y = 110;

    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            pd->graphics->drawRect(start_x + i * 48 + j * 12, start_y - j * 42, GRID_CELL_SIZE, GRID_CELL_SIZE, kColorBlack);
        }
    }
    
    start_x = 350;

    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 3; j++) {
            pd->graphics->drawRect(start_x - i * 48 - j * 12, start_y - j * 42, GRID_CELL_SIZE, GRID_CELL_SIZE, kColorBlack);
            if(i == 1) {
                LCDBitmap* m = pd->graphics->getTableBitmap(monsters, j);
                pd->graphics->drawBitmap(m, start_x + 2 - i * 48 - j * 12, start_y + 2 - j * 42, kBitmapUnflipped);
            }
        }
    }  
}

void battleOnExit(void* params) {
    return;
}

void battleUpdate(void* params, float dt) {
    BattleParams* battleParams = (BattleParams*)params;
    PlaydateAPI* pd = battleParams->pd;

    PDButtons btn_pressed;
    pd->system->getButtonState(NULL, &btn_pressed, NULL);

    int dx = 0, dy = 0;
    if (btn_pressed & kButtonLeft)      dx = -1;
    else if (btn_pressed & kButtonRight)  dx = 1;
    else if (btn_pressed & kButtonUp)     dy = 1;
    else if (btn_pressed & kButtonDown)   dy = -1;
    
    if (dx || dy) {
        battleParams->selectX = (battleParams->selectX + dx + 5) % 5;
        battleParams->selectY = (battleParams->selectY + dy + 3) % 3;
    } 

    battleDraw(params);
}

void battleDraw(void* params) {
    BattleParams* battleParams = (BattleParams*)params;
    PlaydateAPI* pd = battleParams->pd;
    int offset = (GRID_CELL_SIZE - 32) / 2;
    int selectX, selectY;
    
    if (battleParams->selectX < 3) {
        selectX = 14 + battleParams->selectX * 48 + battleParams->selectY * 12 + offset;
        selectY = 110 - battleParams->selectY * 42 + offset;
    } else {
        int rightCol = battleParams->selectX - 3;
        selectX = 302 + rightCol * 48 - battleParams->selectY * 12 + offset;
        selectY = 110 - battleParams->selectY * 42 + offset;
    }
    
    pd->graphics->clear(kColorWhite);
    battleOnEnter(params);

    LCDBitmap* s = pd->graphics->getTableBitmap(battleParams->select, 1);
    pd->graphics->drawBitmap(s, selectX, selectY, kBitmapUnflipped);

    if(battleParams->selectX == 3) {
        pd->graphics->drawText(enemies[0].name, strlen(enemies[0].name), kASCIIEncoding, 50, 220);
    }
}