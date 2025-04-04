#include "fight.h"

void handlePlayerMenu(BattleParams* battleParams, float dt) {
    PlaydateAPI* pd = battleParams->pd;
    PDButtons btn_pressed;
    pd->system->getButtonState(NULL, &btn_pressed, NULL);

    if (btn_pressed & kButtonA || battleParams->exitMenu || battleParams->enterMenu) {
        if(battleParams->enterMenu) {
            for (int i = 0; i < 4; i++) {
                int x = menuPositions[i][0];
                int y = menuPositions[i][1] + (int)(battleParams->menuOffset);
                pd->graphics->fillRect(x - 2, y - 2, 129, 34, kColorWhite);
            }
            battleParams->menuOffset -= dt * 200.0f;
            if (battleParams->menuOffset <= 0) {
                battleParams->menuOffset = 0.0;
                battleParams->enterMenu = false;
            }
            for (int i = 0; i < 4; i++) {
                int x = menuPositions[i][0];
                int y = menuPositions[i][1] + (int)(battleParams->menuOffset);
                pd->graphics->drawRect(x, y, BUTTON_WIDTH, BUTTON_HEIGHT, kColorBlack);
                pd->graphics->drawText(menuOptions[i], strlen(menuOptions[i]), kASCIIEncoding, x + BUTTON_TEXT_OFFSET_X, y + BUTTON_TEXT_OFFSET_Y);
                if (i == battleParams->menuIndex) {
                    pd->graphics->fillRect(x - 1, y - 1, 127, 32, kColorXOR);
                }
            }
            return;
        }
        if (battleParams->exitMenu) {
            for (int i = 0; i < 4; i++) {
                int x = menuPositions[i][0];
                int y = menuPositions[i][1] + (int)(battleParams->menuOffset);
                pd->graphics->fillRect(x - 1, y - 1, 127, 32, kColorWhite);
            }
            battleParams->menuOffset += dt * 200.0f;
            if (battleParams->menuOffset >= 180) {
                Player* player = battleParams->players[battleParams->activePlayerIndex];
                for (int j = 0; j < player->attack_count; j++) {
                    int dest_x = (j % 2) ? 255 : 20;
                    int dest_y = (j / 2) ? 200 : 160;
                    player->attacks[j].rect_y = (float)(dest_y + 80);
                    player->attacks[j].dest_y = dest_y;        
                    player->attacks[j].rect_x = dest_x;
                }
                switch (battleParams->menuIndex) {
                    case 0: {
                        int newSelectX = selectPositions[battleParams->selectX][battleParams->selectY][0];
                        int newSelectY = selectPositions[battleParams->selectX][battleParams->selectY][1];
                        LCDBitmap* s = pd->graphics->getTableBitmap(battleParams->selectorIcons, 2);
                        pd->graphics->drawBitmap(s, newSelectX - 30, newSelectY, kBitmapUnflipped);
                        battleParams->infoTargetPlayer = NULL;
                        battleParams->currentState = PLAYER_SELECT_INFO;
                        break;
                    }
                    case 1:
                        battleParams->currentState = PLAYER_ATTACK_SELECTION_ANIMATION;
                        break;
                    case 2:
                        battleParams->currentState = BATTLE_ESCAPE;
                        break;
                    case 3: {
                        LCDBitmap* s = pd->graphics->getTableBitmap(battleParams->selectorIcons, 2);
                        int newSelectX = selectPositions[battleParams->selectX][battleParams->selectY][0];
                        int newSelectY = selectPositions[battleParams->selectX][battleParams->selectY][1];
                        pd->graphics->drawBitmap(s, newSelectX - 30, newSelectY, kBitmapUnflipped);
                        battleParams->selectPlayerIndex = -1;
                        battleParams->currentState = PLAYER_MOVE;
                        break;
                    }
                }
                battleParams->exitMenu = false;
                return;
            }

            for (int i = 0; i < 4; i++) {
                int x = menuPositions[i][0];
                int y = menuPositions[i][1] + (int)(battleParams->menuOffset);
                pd->graphics->drawRect(x, y, BUTTON_WIDTH, BUTTON_HEIGHT, kColorBlack);
                pd->graphics->drawText(menuOptions[i], strlen(menuOptions[i]), kASCIIEncoding, x + BUTTON_TEXT_OFFSET_X, y + BUTTON_TEXT_OFFSET_Y);
                if (i == battleParams->menuIndex) {
                    pd->graphics->fillRect(x - 1, y - 1, 127, 32, kColorXOR);
                }
            }
        } else {
            battleParams->exitMenu = true;
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
        pd->graphics->fillRect(0, 155, 400, 85, kColorWhite);
        int oldSelectX = selectPositions[battleParams->selectX][battleParams->selectY][0];
        int oldSelectY = selectPositions[battleParams->selectX][battleParams->selectY][1];
        pd->graphics->fillRect(oldSelectX - 12, oldSelectY + 12, 8, 8, kColorWhite);
        battleParams->currentState = PLAYER_TURN_INIT;
        return;
    }
    
    Player* p = selectPlayer(battleParams, pd, true);
    if (p == NULL) {
        battleParams->infoTargetPlayer = NULL;
    }
    else if (p != battleParams->infoTargetPlayer) {
        battleParams->infoTargetPlayer = p;
        pd->graphics->fillRect(0, 155, 400, 85, kColorWhite);
        pd->graphics->drawBitmap(battleParams->characterInfoBitmap, 0, 156, kBitmapUnflipped);
        drawFillHealthbar(pd, p->health, p->maxHealth);
        char *s;
        pd->system->formatString(&s, "%s", p->name);
        pd->graphics->drawText(s, strlen(s), kASCIIEncoding, 102, 163);
    }
}

void handleAction(BattleParams* battleParams, PlaydateAPI* pd, Attack attack) {
    battleParams->currentAttack = attack;
    pd->system->logToConsole("Handle Action");
    battleParams->currentState = PLAYER_ATTACK_SELECTION_ANIMATION_REVERSE;
    Player* player = battleParams->players[battleParams->activePlayerIndex];
    for (int j = 0; j < player->attack_count; j++) {
        int dest_x = (j % 2) ? 255 : 20;
        int dest_y = (j / 2) ? 200 : 160;
        player->attacks[j].dest_y = (dest_y + 80);
        player->attacks[j].rect_x = dest_x;
    }
    switch (attack.type) {
        case BASIC_ATTACK:
        case PIERCING_ATTACK:
        case FIRE_ATTACK:
        case WATER_ATTACK:
        case GROUND_ATTACK:
        case DEBUFF_ARMOR:
            battleParams->nextState = PLAYER_SELECT_TARGET_ENEMY;
            break;
        case BUFF_HEAL:
            battleParams->nextState = PLAYER_SELECT_TARGET_ALLY;
            break;
        default:
            break;
    }
}

void assertAction(BattleParams *battleParams) {
    battleParams->pd->system->logToConsole("Assert attack, pattern: %d", battleParams->currentAttack.pattern);
    Enemy *e[4] = {NULL, NULL, NULL, NULL};

    switch (battleParams->currentAttack.pattern) {
        case SINGLE_TARGET: {
            e[0] = getEnemyAtPos(battleParams, battleParams->selectX, battleParams->selectY);
            break;
        }
        case ROW: {
            e[0] = getEnemyAtPos(battleParams, 3, battleParams->selectY);
            e[1] = getEnemyAtPos(battleParams, 4, battleParams->selectY);

            break;
        }
        case COLUMN: {
            e[0] = getEnemyAtPos(battleParams, battleParams->selectX, 0);
            e[1] = getEnemyAtPos(battleParams, battleParams->selectX, 1);
            e[2] = getEnemyAtPos(battleParams, battleParams->selectX, 2);
            break;
        }
        case AREA_ROW: {
            break;
        }
        case AREA_COL: {

            break;
        }
        case AREA_ROW_COL: {

            break;
        }
        default:
            break;
    }

    for(int i = 0; i < 4; i++) {
        if(e[i] == NULL) continue;
        int dmg = attackEnemy(e[i], battleParams->currentAttack);
        if(!e[i]->isAlive) {
            battleParams->pd->system->logToConsole("Enemy %d is dead!", i);
            battleParams->triggerEnemyRedraw = true;
        }
        battleParams->pd->system->logToConsole("Damage to enemy %s: %d, now has %d hp!", e[i]->name, dmg, e[i]->hp);
    }
    battleParams->currentState = FIGHT_LOOP;
}

void handlePlayerSelectTargetAlly(BattleParams *battleParams) {
    PlaydateAPI *pd = battleParams->pd;
    PDButtons btn_pressed;
    pd->system->getButtonState(NULL, &btn_pressed, NULL);
    
    if(btn_pressed & kButtonA) {
        pd->graphics->fillRect(0, 155, 400, 85, kColorWhite);
        battleParams->currentState = ASSERT_ATTACK;
    } else if(btn_pressed & kButtonB) {
        pd->graphics->fillRect(0, 155, 400, 85, kColorWhite);
        battleParams->currentState = PLAYER_ATTACK_SELECTION;
    }

    int dx = 0, dy = 0;
    getDirection(btn_pressed, &dx, &dy);
    
    if (dx || dy) {
        int oldSelectX = selectPositions[battleParams->selectX][battleParams->selectY][0];
        int oldSelectY = selectPositions[battleParams->selectX][battleParams->selectY][1];
        pd->graphics->fillRect(oldSelectX - 12, oldSelectY + 12, 8, 8, kColorWhite);
        battleParams->selectX = 0;
        battleParams->selectY = (battleParams->selectY + dy + 3) % 3;

        int newSelectX = selectPositions[battleParams->selectX][battleParams->selectY][0];
        int newSelectY = selectPositions[battleParams->selectX][battleParams->selectY][1];
        LCDBitmap *s = pd->graphics->getTableBitmap(battleParams->selectorIcons, 2);
        pd->graphics->drawBitmap(s, newSelectX - 30, newSelectY, kBitmapUnflipped);
        pd->graphics->fillRect(0, 155, 400, 85, kColorWhite);
        drawActionAndTargetTooltips(battleParams, pd);
    }
}

void handlePlayerSelectTargetEnemy(BattleParams *battleParams) {
    PlaydateAPI *pd = battleParams->pd;
    PDButtons btn_pressed;
    pd->system->getButtonState(NULL, &btn_pressed, NULL);

    if(btn_pressed & kButtonA) {
        pd->graphics->fillRect(0, 155, 400, 85, kColorWhite);
        battleParams->currentState = ASSERT_ATTACK;
    } else if(btn_pressed & kButtonB) {
        pd->graphics->fillRect(0, 155, 400, 85, kColorWhite);
        battleParams->currentState = PLAYER_ATTACK_SELECTION;
    }

    int dx = 0, dy = 0;
    getDirection(btn_pressed, &dx, &dy); 
    if (dx || dy) {
        int oldSelectX = selectPositions[battleParams->selectX][battleParams->selectY][0];
        int oldSelectY = selectPositions[battleParams->selectX][battleParams->selectY][1];
        pd->graphics->fillRect(oldSelectX - 12, oldSelectY + 12, 8, 8, kColorWhite);
        battleParams->selectX = 1 + ((battleParams->selectX - 1 + dx) % 2);
        battleParams->selectY = (battleParams->selectY + dy + 3) % 3;
    
        int newSelectX = selectPositions[battleParams->selectX][battleParams->selectY][0];
        int newSelectY = selectPositions[battleParams->selectX][battleParams->selectY][1];
        LCDBitmap* s = pd->graphics->getTableBitmap(battleParams->selectorIcons, 2);
        pd->graphics->drawBitmap(s, newSelectX - 30, newSelectY, kBitmapUnflipped);
        pd->graphics->fillRect(0, 155, 400, 85, kColorWhite);
        drawActionAndTargetTooltips(battleParams, pd);
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
        Player* p = battleParams->players[battleParams->activePlayerIndex];
        handleAction(battleParams, battleParams->pd, p->attacks[battleParams->menuIndex]);
    } else if (btn_pressed & kButtonB) {
        battleParams->enterMenu = true;
        battleParams->currentState = PLAYER_ATTACK_SELECTION_ANIMATION_REVERSE;
        battleParams->nextState = PLAYER_TURN_INIT;
        Player* player = battleParams->players[battleParams->activePlayerIndex];
        for (int j = 0; j < player->attack_count; j++) {
            int dest_x = (j % 2) ? 255 : 20;
            int dest_y = (j / 2) ? 200 : 160;
            player->attacks[j].dest_y = (dest_y + 80);
            player->attacks[j].rect_x = dest_x;
        }
    }
}

void handleBattleEscape(BattleParams* battleParams) {
    battleParams->currentState = PLAYER_MENU;
}

void handlePlayerMove(BattleParams* battleParams) {
    PlaydateAPI* pd = battleParams->pd;
    PDButtons btn_pressed;
    pd->system->getButtonState(NULL, &btn_pressed, NULL);


    battleParams->currentState = PLAYER_TURN_INIT;
    return;
}

void handleFightLoop(BattleParams* battleParams) {

    while ( battleParams->sequence[battleParams->currSequencePos] >= 3 ) {
        battleParams->currSequencePos = (battleParams->currSequencePos + 1) % (3 + battleParams->enemyCount); 
    }

    battleParams->currentState = PLAYER_TURN_INIT;
    battleParams->pd->system->logToConsole("Current Sequence pos: %d", battleParams->currSequencePos);

    // battleParams->currSequencePos++;
    // if(battleParams->currSequencePos >= (3 + battleParams->enemyCount)) {
    //     battleParams->currSequencePos = 0;
    // }
    // if (battleParams->currSequencePos < 3) {
    //     battleParams->currentState = PLAYER_TURN_INIT;
    // } else {
    //     battleParams->currentState = ENEMY_TURN;
    // }
}