#include "fight.h"

void handlePlayerMenu(BattleParams* battleParams, float dt) {
    PlaydateAPI* pd = battleParams->pd;
    PDButtons btn_pressed;
    pd->system->getButtonState(NULL, &btn_pressed, NULL);

    if (btn_pressed & kButtonA || battleParams->exit_menu) {
        if (battleParams->exit_menu) {
            for (int i = 0; i < 4; i++) {
                int x = menuPositions[i][0];
                int y = menuPositions[i][1] + (int)(battleParams->menu_offset);
                pd->graphics->fillRect(x - 1, y - 1, 127, 32, kColorWhite);
            }
            battleParams->menu_offset += dt * 200.0f;
            if (battleParams->menu_offset >= 180) {
                switch (battleParams->menuIndex) {
                    case 0: {
                        int newSelectX = selectPositions[battleParams->selectX][battleParams->selectY][0];
                        int newSelectY = selectPositions[battleParams->selectX][battleParams->selectY][1];
                        LCDBitmap* s = pd->graphics->getTableBitmap(battleParams->select, 2);
                        pd->graphics->drawBitmap(s, newSelectX - 30, newSelectY, kBitmapUnflipped);
                        battleParams->infoPlayer = NULL;
                        battleParams->state = PLAYER_SELECT_INFO;
                        break;
                    }
                    case 1:
                        battleParams->state = PLAYER_ATTACK_SELECTION_ANIMATION;
                        break;
                    case 2:
                        battleParams->state = BATTLE_ESCAPE;
                        break;
                    case 3: {
                        LCDBitmap* s = pd->graphics->getTableBitmap(battleParams->select, 2);
                        int newSelectX = selectPositions[battleParams->selectX][battleParams->selectY][0];
                        int newSelectY = selectPositions[battleParams->selectX][battleParams->selectY][1];
                        pd->graphics->drawBitmap(s, newSelectX - 30, newSelectY, kBitmapUnflipped);
                        battleParams->selectP = -1;
                        battleParams->state = PLAYER_MOVE;
                        break;
                    }
                }
                battleParams->exit_menu = false;
                return;
            }

            for (int i = 0; i < 4; i++) {
                int x = menuPositions[i][0];
                int y = menuPositions[i][1] + (int)(battleParams->menu_offset);
                pd->graphics->drawRect(x, y, BUTTON_WIDTH, BUTTON_HEIGHT, kColorBlack);
                pd->graphics->drawText(menuOptions[i], strlen(menuOptions[i]), kASCIIEncoding, x + BUTTON_TEXT_OFFSET_X, y + BUTTON_TEXT_OFFSET_Y);
                if (i == battleParams->menuIndex) {
                    pd->graphics->fillRect(x - 1, y - 1, 127, 32, kColorXOR);
                }
            }
        } else {
            battleParams->exit_menu = true;
        }
        return;
    }

    if (btn_pressed & (kButtonLeft | kButtonRight | kButtonUp | kButtonDown)) {
        int x = menuPositions[battleParams->menuIndex][0];
        int y = menuPositions[battleParams->menuIndex][1];
        pd->graphics->fillRect(x - 1, y - 1, 127, 32, kColorXOR);

        int dx = 0, dy = 0;
        getDirection(btn_pressed, &dx, &dy);
        int col = battleParams->menuIndex % 2;
        int row = battleParams->menuIndex / 2;
        if (dx) col = 1 - col;
        if (dy) row = 1 - row;
        battleParams->menuIndex = row * 2 + col;

        x = menuPositions[battleParams->menuIndex][0];
        y = menuPositions[battleParams->menuIndex][1];
        pd->graphics->fillRect(x - 1, y - 1, 127, 32, kColorXOR);
    }
}

void handlePlayerSelectInfo(BattleParams* battleParams, float dt) {
    PlaydateAPI* pd = battleParams->pd;

    PDButtons btn_pressed;
    pd->system->getButtonState(NULL, &btn_pressed, NULL);
    
    if (btn_pressed & kButtonB) {
        int oldSelectX = selectPositions[battleParams->selectX][battleParams->selectY][0];
        int oldSelectY = selectPositions[battleParams->selectX][battleParams->selectY][1];
        pd->graphics->fillRect(oldSelectX - 12, oldSelectY + 12, 8, 8, kColorWhite);
        battleParams->state = PLAYER_TURN_INIT;
        return;
    }
    
    Player* p = selectPlayer(battleParams, pd, true);
    if (p != NULL && p != battleParams->infoPlayer) {
        battleParams->infoPlayer = p;
        pd->graphics->fillRect(0, 155, 400, 85, kColorWhite);
        pd->graphics->drawBitmap(battleParams->character_info, 0, 156, kBitmapUnflipped); 
        drawFillHealthbar(pd, p->health, p->maxHealth);
        char *s;
        pd->system->formatString(&s, "%s", p->name);
        pd->graphics->drawText(s, strlen(s), kASCIIEncoding, 102, 163);
    }
}

void handleAction(BattleParams* battleParams, PlaydateAPI* pd, Attack attack) {
    switch (attack.type) {
        case BASIC_ATTACK:
            break;

        case PIERCING_ATTACK:
            break;

        case FIRE_ATTACK:
            break;

        case WATER_ATTACK:
            break;

        case GROUND_ATTACK:
            break;

        case BUFF_HEAL:    
            pd->graphics->fillRect(battleParams->selectX - 12, battleParams->selectY + 12, 8, 8, kColorWhite);
            battleParams->selectX = 0;
            battleParams->selectY = 0;
            battleParams->state = PLAYER_SELECT_PLAYER;
            break;
        case DEBUFF_ARMOR:
            break;
        default:
            break;
    }
}

void handlePlayerAttackSelection(BattleParams* battleParams, float dt) {
    PDButtons btn_pressed;
    battleParams->pd->system->getButtonState(NULL, &btn_pressed, NULL);
    
    if (btn_pressed & (kButtonLeft | kButtonRight | kButtonUp | kButtonDown)) {
        int dx = 0, dy = 0;
        getDirection(btn_pressed, &dx, &dy);
        int col = battleParams->menuIndex % 2;
        int row = battleParams->menuIndex / 2;
        if (dx) col = 1 - col;
        if (dy) row = 1 - row;
        battleParams->menuIndex = row * 2 + col;
        drawAttackOptions(battleParams, battleParams->pd);
    }

    if (btn_pressed & kButtonA) {
        Player* p = battleParams->chars[battleParams->activeP];
        handleAction(battleParams, battleParams->pd, p->attacks[battleParams->menuIndex]);
    } else if (btn_pressed & kButtonB) {
        battleParams->state = PLAYER_TURN_INIT;
        Player* player = battleParams->chars[battleParams->activeP];
        player->sprite.frameTimer = 0.0;
        player->sprite.currentFrame = 0;
        for (int j = 0; j < player->attack_count; j++) {
            int dest_x = (j % 2) ? 255 : 20;
            int dest_y = (j / 2) ? 200 : 160;
            player->attacks[j].rect_y = (float)(dest_y + 80);
            player->attacks[j].dest_y = dest_y;
            player->attacks[j].rect_x = dest_x;
        }
    }
}

void handleBattleEscape(BattleParams* battleParams) {
    battleParams->state = PLAYER_MENU;
}

void handlePlayerMove(BattleParams* battleParams) {
    PlaydateAPI* pd = battleParams->pd;
    PDButtons btn_pressed;
    pd->system->getButtonState(NULL, &btn_pressed, NULL);

    if(btn_pressed & kButtonB) {
        battleParams->state = PLAYER_TURN_INIT;
        int prevSelectX = selectPositions[battleParams->selectX][battleParams->selectY][0];
        int prevSelectY = selectPositions[battleParams->selectX][battleParams->selectY][1];
        pd->graphics->fillRect(prevSelectX - 12, prevSelectY + 12, 8, 8, kColorWhite);
        return;
    }

    if(btn_pressed & kButtonA) {
        if(battleParams->selectP == -1) {
            for (int i = 0; i < NUM_PLAYERS; i++) {
                Player* p = battleParams->chars[i];
                if (p && p->fight_x == battleParams->selectX && p->fight_y == battleParams->selectY) {
                    battleParams->selectP = i;
                }
            }
        } else {
            battleParams->selectP = -1;
        }
    }

    int dx = 0, dy = 0;
    getDirection(btn_pressed, &dx, &dy);
    if (!dx && !dy) return;

    int newX = (battleParams->selectX + dx + 3) % 3;
    int newY = (battleParams->selectY + dy + 3) % 3;


    if(battleParams->selectP != -1) {
        for (int i = 0; i < NUM_PLAYERS; i++) {
            if (i != battleParams->selectP && battleParams->chars[i] &&
                battleParams->chars[i]->fight_x == newX && battleParams->chars[i]->fight_y == newY) {
                return;
            }
        }
    }

    int prevSelectX = selectPositions[battleParams->selectX][battleParams->selectY][0];
    int prevSelectY = selectPositions[battleParams->selectX][battleParams->selectY][1];
    pd->graphics->fillRect(prevSelectX - 12, prevSelectY + 12, 8, 8, kColorWhite);
    
    battleParams->selectX = newX;
    battleParams->selectY = newY;
    if(battleParams->selectP != -1) {
        pd->graphics->fillRect(prevSelectX - 1, prevSelectY - 1, 34, 34, kColorWhite);
        battleParams->chars[battleParams->selectP]->fight_x = newX;
        battleParams->chars[battleParams->selectP]->fight_y = newY;
    }
    LCDBitmap* s = pd->graphics->getTableBitmap(battleParams->select, 2);

    int newSelectX = selectPositions[battleParams->selectX][battleParams->selectY][0];
    int newSelectY = selectPositions[battleParams->selectX][battleParams->selectY][1];
    pd->graphics->drawBitmap(s, newSelectX - 30, newSelectY, kBitmapUnflipped);
}

void handleFightLoop(BattleParams* battleParams) {
    battleParams->currSequencePos++;
    if (battleParams->currSequencePos < 3) {
        battleParams->state = PLAYER_MENU;
    } else {
        battleParams->state = ENEMY_TURN;
    }
}