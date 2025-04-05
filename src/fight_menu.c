#include "fight.h"

void handlePlayerMenu(BattleParams* battleParams, float dt) {
    PlaydateAPI* pd = battleParams->pd;
    PDButtons btn_pressed;
    pd->system->getButtonState(NULL, &btn_pressed, NULL);

    if (btn_pressed & kButtonA) {
        switch (battleParams->menuIndex) {
            case 0:
                battleParams->currentState = PLAYER_ATTACK_SELECTION;
                drawAttackOptions(battleParams, pd);
                break;
            case 1: {
                drawSelector(battleParams);
                battleParams->infoTargetPlayer = NULL;
                battleParams->currentState = PLAYER_SELECT_INFO;
                break;
            }
            case 2:
                battleParams->currentState = BATTLE_ESCAPE;
                break;
            case 3: {
                drawSelector(battleParams);
                battleParams->selectPlayerIndex = -1;
                battleParams->currentState = PLAYER_MOVE;
                break;
            }
        }
    }

    int dx = 0, dy = 0;
    getDirection(btn_pressed, &dx, &dy); 
    if (dx || dy) {
        battleParams->menuIndex = (battleParams->menuIndex + dx + 2 * dy + 4) % 4;
        drawPlayerMenu(battleParams, pd);
    }
}

void handlePlayerSelectInfo(BattleParams* battleParams, float dt) {
    PlaydateAPI* pd = battleParams->pd;

    PDButtons btn_pressed;
    pd->system->getButtonState(NULL, &btn_pressed, NULL);
    
    if (btn_pressed & kButtonB) {
        int oldSelectX, oldSelectY;
        clearInfoArea(pd);
        getGridPosition(battleParams->selectX, battleParams->selectY, &oldSelectX, &oldSelectY);
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
        clearInfoArea(pd);
        pd->graphics->drawBitmap(battleParams->characterInfoBitmap, 0, 156, kBitmapUnflipped);
        drawFillHealthbar(pd, p->health, p->maxHealth);
        char *s;
        pd->system->formatString(&s, "%s", p->name);
        pd->graphics->drawText(s, strlen(s), kASCIIEncoding, 102, 163);
    }
}

void handleAction(BattleParams* battleParams, PlaydateAPI* pd, Attack attack) {
    battleParams->currentAttack = attack;
    switch (attack.type) {
        case BASIC_ATTACK:
        case PIERCING_ATTACK:
        case FIRE_ATTACK:
        case WATER_ATTACK:
        case GROUND_ATTACK:
        case DEBUFF_ARMOR: {
            battleParams->currentState = PLAYER_SELECT_TARGET_ENEMY;
            int newSelectX, newSelectY;
            getGridPosition(battleParams->selectX, battleParams->selectY, &newSelectX, &newSelectY);
            for(int i = 0; i < battleParams->enemyCount; i++) {
                if (battleParams->enemies[i].isAlive) {
                    battleParams->selectX = battleParams->enemies[i].fight_x;
                    battleParams->selectY = battleParams->enemies[i].fight_y;
                    getGridPosition(battleParams->selectX, battleParams->selectY, &newSelectX, &newSelectY);
                    break;
                }
            }
            LCDBitmap* s = pd->graphics->getTableBitmap(battleParams->selectorIcons, 2);
            pd->graphics->drawBitmap(s, newSelectX - 30, newSelectY, kBitmapUnflipped);
            drawActionAndTargetTooltips(battleParams, pd);
            break;
        }
        case BUFF_HEAL: {
            battleParams->currentState = PLAYER_SELECT_TARGET_ALLY;
            int newSelectX, newSelectY;
            getGridPosition(battleParams->selectX, battleParams->selectY, &newSelectX, &newSelectY);
            LCDBitmap* s = pd->graphics->getTableBitmap(battleParams->selectorIcons, 2);
            pd->graphics->drawBitmap(s, newSelectX - 30, newSelectY, kBitmapUnflipped);
            break;
        }
        default:
            break; 
    }
}

void assertAction(BattleParams *battleParams) {
    Enemy *e[6] = {NULL, NULL, NULL, NULL, NULL, NULL};

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
        case AREA_ROW_COL: {
            e[0] = getEnemyAtPos(battleParams, 1, 0);
            e[1] = getEnemyAtPos(battleParams, 1, 1);
            e[2] = getEnemyAtPos(battleParams, 1, 2);
            e[3] = getEnemyAtPos(battleParams, 2, 0);
            e[4] = getEnemyAtPos(battleParams, 2, 1);
            e[5] = getEnemyAtPos(battleParams, 2, 2);
            break;
        }
        default:
            break;
    }

    char *c = NULL;
    for (int i = 0; i < 4; i++) {
        if (e[i] == NULL) continue;
        e[i]->hit = true;
        int dmg = attackEnemy(e[i], battleParams->currentAttack);
        char *newString;
        if (!e[i]->isAlive) {
            battleParams->triggerEnemyRedraw = true;
            battleParams->pd->system->formatString(&newString, "%s'%s' took %d damage and died!\n", c ? c : "", e[i]->name, dmg);
        } else {
            battleParams->pd->system->formatString(&newString, "%s'%s' took %d damage!\n", c ? c : "", e[i]->name, dmg);
        }
    
        if (c) {
            battleParams->pd->system->realloc(c, 0);
        }
        c = newString; 
    }
    if(c != NULL) {
        strcpy(battleParams->areaText, c);
    } else {
        strcpy(battleParams->areaText, "Missed!");
    }
    battleParams->pd->system->realloc(c, 0);
    battleParams->elapsedEnemiesHitTime = 0.0f;
    battleParams->enemyHitLastOffsetChangeTime = -1.0f;
    battleParams->elapsedTimeText = 0.0f;
    battleParams->enemyHitOffsetX = 0;
    battleParams->enemyHitOffsetY = 0;
    battleParams->currentState = ENEMIES_HIT;
}

void handlePlayerSelectTargetAlly(BattleParams *battleParams) {
    PlaydateAPI *pd = battleParams->pd;
    PDButtons btn_pressed;
    pd->system->getButtonState(NULL, &btn_pressed, NULL);
    
    if(btn_pressed & (kButtonA | kButtonB)) {
        clearInfoArea(pd);
        int oldSelectX, oldSelectY;
        getGridPosition(battleParams->selectX, battleParams->selectY, &oldSelectX, &oldSelectY);
        pd->graphics->fillRect(oldSelectX - 12, oldSelectY + 12, 8, 8, kColorWhite);
        battleParams->currentState = (btn_pressed & kButtonA) ? ASSERT_ATTACK : PLAYER_ATTACK_SELECTION;
    }

    int dx = 0, dy = 0;
    getDirection(btn_pressed, &dx, &dy);
    
    if (dx || dy) {
        int oldSelectX, oldSelectY;
        getGridPosition(battleParams->selectX, battleParams->selectY, &oldSelectX, &oldSelectY);
        pd->graphics->fillRect(oldSelectX - 12, oldSelectY + 12, 8, 8, kColorWhite);
        battleParams->selectX = 0;
        battleParams->selectY = (battleParams->selectY + dy + 3) % 3;

        int newSelectX, newSelectY;
        getGridPosition(battleParams->selectX, battleParams->selectY, &newSelectX, &newSelectY);
        LCDBitmap *s = pd->graphics->getTableBitmap(battleParams->selectorIcons, 2);
        pd->graphics->drawBitmap(s, newSelectX - 30, newSelectY, kBitmapUnflipped);
        clearInfoArea(pd);
        drawActionAndTargetTooltips(battleParams, pd);
    }
}

void handlePlayerSelectTargetEnemy(BattleParams *battleParams) {
    PlaydateAPI *pd = battleParams->pd;
    PDButtons btn_pressed;
    pd->system->getButtonState(NULL, &btn_pressed, NULL);

    int oldSelectX, oldSelectY;
    if (btn_pressed & (kButtonA | kButtonB)) {
        clearInfoArea(pd);
        getGridPosition(battleParams->selectX, battleParams->selectY, &oldSelectX, &oldSelectY);
        pd->graphics->fillRect(oldSelectX - 12, oldSelectY + 12, 8, 8, kColorWhite);
        
        if(btn_pressed & kButtonA) {
            battleParams->currentState = ASSERT_ATTACK;
        } else {
            battleParams->currentState = PLAYER_ATTACK_SELECTION;
            drawAttackOptions(battleParams, battleParams->pd);
        }
        return;
    }

    int dx = 0, dy = 0;
    getDirection(btn_pressed, &dx, &dy); 
    if (dx || dy) {
        getGridPosition(battleParams->selectX, battleParams->selectY, &oldSelectX, &oldSelectY);
        pd->graphics->fillRect(oldSelectX - 12, oldSelectY + 12, 8, 8, kColorWhite);

        battleParams->selectX += dx;
        if (battleParams->selectX < 1) battleParams->selectX = 2;
        if (battleParams->selectX > 2) battleParams->selectX = 1;

        battleParams->selectY += dy;
        if (battleParams->selectY < 0) battleParams->selectY = 2;
        if (battleParams->selectY > 2) battleParams->selectY = 0;

        int newSelectX, newSelectY;
        getGridPosition(battleParams->selectX, battleParams->selectY, &newSelectX, &newSelectY);
        LCDBitmap* s = pd->graphics->getTableBitmap(battleParams->selectorIcons, 2);
        pd->graphics->drawBitmap(s, newSelectX - 30, newSelectY, kBitmapUnflipped);

        clearInfoArea(pd);
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
        battleParams->currentState = HANDLE_ACTION;
    } else if (btn_pressed & kButtonB) {
        battleParams->currentState = PLAYER_TURN_INIT;
        drawPlayerMenu(battleParams, battleParams->pd);
    }
}

void handleBattleEscape(BattleParams* battleParams) {
    battleParams->currentState = PLAYER_MENU;
}

void handlePlayerMove(BattleParams* battleParams) {
    battleParams->currentState = PLAYER_TURN_INIT;
    return;
}

void handleFightLoop(BattleParams* battleParams) {
    battleParams->currSequencePos = (battleParams->currSequencePos + 1) % (3 + battleParams->enemyCount);
    if (battleParams->sequence[battleParams->currSequencePos] < 3) {
        battleParams->activePlayerIndex = battleParams->sequence[battleParams->currSequencePos];
        battleParams->currentState = PLAYER_TURN_INIT;
        drawPlayerMenu(battleParams, battleParams->pd);
    } else {
        battleParams->currentState = ENEMY_TURN;
    }
}

void handleEnemyTurn(BattleParams* battleParams) {
    Enemy e = battleParams->enemies[battleParams->sequence[battleParams->currSequencePos] - 3];

    int rAttack = rand() % e.attackCount;
    int countPlayer = 0;
    for(int i = 0; i < 3; i++) {
        if(battleParams->players[i]->isAlive) countPlayer++;
    }
    int rPlayer = rand() % countPlayer;
    
    int dmg = takeDamage(battleParams->players[rPlayer], e.attacks[rAttack]);

    char *c = NULL;
    battleParams->pd->system->formatString(&c, "'%s' took %d damage from '%s' casting '%s'!", battleParams->players[rPlayer]->name, dmg, e.name, e.attacks[rAttack].name);
    if(c != NULL) {
        strcpy(battleParams->areaText, c);
    } else {
        strcpy(battleParams->areaText, "Missed!");
    }
    battleParams->currentState = SHOW_ENEMY_ATTACK;
}