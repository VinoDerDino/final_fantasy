#include "fight.h"

void battleOnEnter(void *params) {
    BattleParams* battleParams = (BattleParams*)params;
    PlaydateAPI* pd = battleParams->pd;
    LCDBitmapTable* monsters = battleParams->monsters;

    
    pd->graphics->clear(kColorWhite);

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

    if(btn_pressed & kButtonA) {
        if(battleParams->state == PLAYER_TURN) {
            for(int i = 0; i < 3; i++) {
                Player* p = battleParams->chars[i];
                if(p != NULL && p->fight_x == battleParams->selectX && p->fight_y == battleParams->selectY) {
                    battleParams->state = PLAYER_MOVE;
                    battleParams->selectP = i;
                    pd->system->logToConsole("Player %d: x = %d, y = %d; Select x = %d, y = %d", i, p->fight_x, p->fight_y, battleParams->selectX, battleParams->selectY);
                }
            }
        } else if(battleParams->state == PLAYER_MOVE) {
            battleParams->state = PLAYER_TURN;
        }
    }

    int dx = 0, dy = 0;
    if (btn_pressed & kButtonLeft)      dx = -1;
    else if (btn_pressed & kButtonRight)  dx = 1;
    else if (btn_pressed & kButtonUp)     dy = 1;
    else if (btn_pressed & kButtonDown)   dy = -1;
    
    if (dx || dy) {
        if(battleParams->state == PLAYER_MOVE) {        
            battleParams->selectX = (battleParams->selectX + dx + 3) % 3;
            battleParams->selectY = (battleParams->selectY + dy + 3) % 3;
            battleParams->chars[battleParams->selectP]->fight_x = battleParams->selectX;
            battleParams->chars[battleParams->selectP]->fight_y = battleParams->selectY;
        } else {
            battleParams->selectX = (battleParams->selectX + dx + 5) % 5;
            battleParams->selectY = (battleParams->selectY + dy + 3) % 3;
        }
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

    for(int i = 0; i < 3; i++) {
        if(battleParams->chars[i] != NULL) {
            selectX = 14 + battleParams->chars[i]->fight_x * 48 + battleParams->chars[i]->fight_y * 12 + offset;
            selectY = 110 - battleParams->chars[i]->fight_y * 42 + offset;
            drawAnimatedSprite(&battleParams->chars[i]->sprite, pd, selectX, selectY, kBitmapUnflipped, 0.0);
        }
    }

    if(battleParams->state == PLAYER_TURN) {
        pd->graphics->drawRect(20, 150, 255, 30, kColorBlack);
        pd->graphics->drawRect(20, 190, 255, 30, kColorBlack);
        pd->graphics->drawRect(170, 150, 255, 30, kColorBlack); 
        pd->graphics->drawRect(170, 190, 255, 30, kColorBlack);
    }
}