#include "fight.h"

bool drawAttackButtonAnimation(void* params, float dt) {
    BattleParams* battleParams = (BattleParams*)params;
    PlaydateAPI* pd = battleParams->pd;
    Player* player = battleParams->chars[battleParams->activeP];

    bool finished = true;
    float speed = 100.0f;

    for(int i = 0; i < 4; i++) {
        pd->graphics->drawRect(player->attacks[i].rect_x, (int)(player->attacks[i].rect_y), 125, 30, kColorBlack);
        pd->graphics->drawText(player->attacks[i].name, strlen(player->attacks[i].name), kASCIIEncoding, player->attacks[i].rect_x + 5, player->attacks[i].rect_y + 7);

        if((int)(player->attacks[i].rect_y) > player->attacks[i].dest_y) {
            finished = false;
            float newY = player->attacks[i].rect_y - (speed * dt);

            if (newY < player->attacks[i].dest_y) {
                player->attacks[i].rect_y = (float)player->attacks[i].dest_y;
            } else {
                player->attacks[i].rect_y = newY;
            }
        }
    }
    return finished;
}

void battleOnEnter(void *params) {
    BattleParams* battleParams = (BattleParams*)params;
    PlaydateAPI* pd = battleParams->pd;
    
    pd->graphics->clear(kColorWhite);

    battleParams->state = PLAYER_TURN_INIT;
    battleParams->activeP = 0;

    for (int i = 0; i < 4; i++) {
        int dest_x = (i % 2) ? 255 : 20;
        int dest_y = (i / 2) ? 200 : 160;
    
        battleParams->chars[battleParams->activeP]->attacks[i].rect_x = dest_x;
        battleParams->chars[battleParams->activeP]->attacks[i].rect_y = (float)(dest_y + 50 + 25 * i);
        battleParams->chars[battleParams->activeP]->attacks[i].dest_y = dest_y;
    }
}

void battleOnExit(void* params) {
    // Freigeben des dynamisch allokierten Speichers für BattleParams
    free(params);
}

void handlePlayerTurnInit(BattleParams* battleParams, float dt) {
    bool finished = drawAttackButtonAnimation(battleParams, dt);
    if (finished) {
        battleParams->state = PLAYER_TURN;
    }
}

void handlePlayerTurn(BattleParams* battleParams) {
    PlaydateAPI* pd = battleParams->pd;
    PDButtons btn_pressed;
    pd->system->getButtonState(NULL, &btn_pressed, NULL);

    if(btn_pressed & kButtonA) {
        for(int i = 0; i < 3; i++) {
            Player* p = battleParams->chars[i];
            if(p != NULL && p->fight_x == battleParams->selectX && p->fight_y == battleParams->selectY) {
                battleParams->state = PLAYER_MOVE;
                battleParams->selectP = i;
            }
        }
    }
}

void handlePlayerMove(BattleParams* battleParams) {
    PlaydateAPI* pd = battleParams->pd;
    PDButtons btn_pressed;
    pd->system->getButtonState(NULL, &btn_pressed, NULL);

    if(btn_pressed & kButtonA) {
        battleParams->state = PLAYER_TURN;
    }

    int dx = 0, dy = 0;
    if (btn_pressed & kButtonLeft)      dx = -1;
    else if (btn_pressed & kButtonRight)  dx = 1;
    else if (btn_pressed & kButtonUp)     dy = 1;
    else if (btn_pressed & kButtonDown)   dy = -1;
    
    if (dx || dy) {
        battleParams->selectX = (battleParams->selectX + dx + 3) % 3;
        battleParams->selectY = (battleParams->selectY + dy + 3) % 3;
        battleParams->chars[battleParams->selectP]->fight_x = battleParams->selectX;
        battleParams->chars[battleParams->selectP]->fight_y = battleParams->selectY;
    }
}

void battleUpdate(void* params, float dt) {
    BattleParams* battleParams = (BattleParams*)params;

    battleDraw(params);

    switch (battleParams->state) {
        case PLAYER_TURN_INIT:
            handlePlayerTurnInit(battleParams, dt);
            break;
        case PLAYER_TURN:
            handlePlayerTurn(battleParams);
            break;
        case PLAYER_MOVE:
            handlePlayerMove(battleParams);
            break;
        default:
            break;
    }

    int offset = (GRID_CELL_SIZE - 32) / 2;
    int selectX, selectY;
    for(int i = 0; i < 3; i++) {
        if(battleParams->chars[i] != NULL) {
            selectX = 14 + battleParams->chars[i]->fight_x * 48 + battleParams->chars[i]->fight_y * 12 + offset;
            selectY = 110 - battleParams->chars[i]->fight_y * 42 + offset;
            drawAnimatedSprite(&battleParams->chars[i]->sprite, battleParams->pd, selectX, selectY, true, kBitmapUnflipped, dt);
        }
    }
}

void battleDraw(void* params) {
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

    LCDBitmap* s = pd->graphics->getTableBitmap(battleParams->select, 1);
    pd->graphics->drawBitmap(s, selectX, selectY, kBitmapUnflipped);

    if(battleParams->selectX == 3) {
        pd->graphics->drawText(enemies[0].name, strlen(enemies[0].name), kASCIIEncoding, 50, 220);
    }

    
    Player* player = battleParams->chars[battleParams->activeP];

    for(int i = 0; i < 4; i++) {
        pd->graphics->drawRect(player->attacks[i].rect_x, (int)(player->attacks[i].rect_y), 125, 30, kColorBlack);
        pd->graphics->drawText(player->attacks[i].name, strlen(player->attacks[i].name), kASCIIEncoding, player->attacks[i].rect_x + 5, player->attacks[i].rect_y + 7); 
    }
}