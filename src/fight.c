#include "fight.h"

static int selectPositions[GRID_SIZE][GRID_SIZE][2];

void initializeSelectPositions(void) {
    const int offset = (GRID_CELL_SIZE - 32) / 2;

    for (int x = 0; x < GRID_SIZE; x++) {
        for (int y = 0; y < GRID_SIZE; y++) {
            if (y > 2) {
                selectPositions[x][y][0] = (x < 3) ? 20 : 255;
                selectPositions[x][y][1] = (y == 3) ? 200 : 160;
            } else {
                if (x < 3) {
                    selectPositions[x][y][0] = 14 + x * 48 + y * 12 + offset;
                    selectPositions[x][y][1] = 110 - y * 42 + offset;
                } else {
                    int rightCol = x - 3;
                    selectPositions[x][y][0] = 302 + rightCol * 48 - y * 12 + offset;
                    selectPositions[x][y][1] = 110 - y * 42 + offset;
                }
            }
        }
    }
}

bool drawAttackButtonAnimation(void* params, float dt) {
    BattleParams* battleParams = (BattleParams*)params;
    PlaydateAPI* pd = battleParams->pd;
    Player* player = battleParams->chars[battleParams->activeP];

    bool finished = true;
    const float speed = 100.0f;

    for (int i = 0; i < 4; i++) {
        pd->graphics->fillRect(player->attacks[i].rect_x, (int)player->attacks[i].rect_y,
                               ATTACK_BTN_WIDTH, ATTACK_BTN_HEIGHT, kColorWhite);

        if ((int)player->attacks[i].rect_y > player->attacks[i].dest_y) {
            finished = false;
            float newY = player->attacks[i].rect_y - (speed * dt);
            player->attacks[i].rect_y = (newY < player->attacks[i].dest_y)
                                            ? (float)player->attacks[i].dest_y
                                            : newY;
        }

        pd->graphics->drawRect(player->attacks[i].rect_x, (int)player->attacks[i].rect_y,
                               ATTACK_BTN_WIDTH, ATTACK_BTN_HEIGHT, kColorBlack);
        pd->graphics->drawText(player->attacks[i].name, strlen(player->attacks[i].name),
                               kASCIIEncoding,
                               player->attacks[i].rect_x + 5,
                               (int)(player->attacks[i].rect_y + 7));
    }
    return finished;
}

void drawGridLeft(PlaydateAPI* pd) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int xPos = GRID_LEFT_START_X + i * 48 + j * 12;
            int yPos = GRID_LEFT_START_Y - j * 42;
            pd->graphics->drawRect(xPos, yPos, GRID_CELL_SIZE, GRID_CELL_SIZE, kColorBlack);
        }
    }
}

void drawGridRight(BattleParams* battleParams, PlaydateAPI* pd) {
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 3; j++) {
            int xPos = GRID_RIGHT_START_X - i * 48 - j * 12;
            int yPos = GRID_RIGHT_START_Y - j * 42;
            pd->graphics->drawRect(xPos, yPos, GRID_CELL_SIZE, GRID_CELL_SIZE, kColorBlack);
            if (i == 1) {
                LCDBitmap *m = pd->graphics->getTableBitmap(battleParams->monsters,
                                                            battleParams->enemies[j].id);
                pd->graphics->drawBitmap(m, xPos + 2, yPos + 2, kBitmapUnflipped);
            }
        }
    }
}

void drawAttackOptions(BattleParams* battleParams, PlaydateAPI* pd) {
    Player* player = battleParams->chars[battleParams->activeP];

    for (int i = 0; i < player->attack_count; i++) {
        pd->graphics->fillRect(player->attacks[i].rect_x - 1, (int)player->attacks[i].rect_y - 1,
                               ATTACK_BTN_WIDTH + 2, ATTACK_BTN_HEIGHT + 2, kColorWhite);
        pd->graphics->drawRect(player->attacks[i].rect_x, (int)player->attacks[i].rect_y,
                               ATTACK_BTN_WIDTH, ATTACK_BTN_HEIGHT, kColorBlack);
        pd->graphics->drawText(player->attacks[i].name, strlen(player->attacks[i].name),
                               kASCIIEncoding,
                               player->attacks[i].rect_x + 5,
                               (int)(player->attacks[i].rect_y + 7));
    }
}

void drawAttackSequence(BattleParams* battleParams, PlaydateAPI* pd) {
    if (!battleParams->sequence) return;

    const int sequenceCount = 3 + battleParams->countMonsters;
    const int offset = ((3 + battleParams->countMonsters) / 2) * 18;
    const int rectX = 196 - offset;
    const int rectWidth = 18 * (3 + battleParams->countMonsters) + 4;

    pd->graphics->fillRect(rectX, -1, rectWidth, 22, kColorWhite);
    pd->graphics->drawRect(rectX, -1, rectWidth, 22, kColorBlack);

    for (int i = 0; i < sequenceCount; i++) {
        if (i >= (int)(sizeof(battleParams->sequence) / sizeof(battleParams->sequence[0])))
            return;

        int curr = battleParams->sequence[i];
        LCDBitmap *m = NULL;
        if (curr < 3) {
            m = pd->graphics->getTableBitmap(battleParams->chars[curr]->sprite.table, 0);
        } else {
            m = pd->graphics->getTableBitmap(battleParams->monsters,
                                             battleParams->enemies[curr - 3].id);
        }
        pd->graphics->drawScaledBitmap(m, (200 - offset) + i * 18, 1, 0.5, 0.5);
        
        if (i == battleParams->currSequencePos && curr < 3) {
            pd->graphics->fillRect((199 - offset) + i * 18, 0, 18, 18, kColorXOR);
        }
    }
}

static void shuffleSequence(int* sequence, int count) {
    for (int i = count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = sequence[i];
        sequence[i] = sequence[j];
        sequence[j] = temp;
    }
}

void battleOnEnter(void *params) {
    BattleParams* battleParams = (BattleParams*)params;
    PlaydateAPI* pd = battleParams->pd;

    pd->graphics->clear(kColorWhite);

    battleParams->state = PLAYER_TURN_INIT;
    battleParams->activeP = 0;
    battleParams->passed_time = 0.0f;

    for (int i = 0; i < 3; i++) {
        Player* player = battleParams->chars[i];
        player->sprite.frameTimer = 0.0;
        player->sprite.currentFrame = 0;
        for (int j = 0; j < player->attack_count; j++) {
            int dest_x = (j % 2) ? 255 : 20;
            int dest_y = (j / 2) ? 200 : 160;
            if (i == battleParams->activeP) {
                player->attacks[j].rect_y = (float)(dest_y + 50 + 25 * j);
                player->attacks[j].dest_y = dest_y;
            } else {
                player->attacks[j].rect_y = (float)(dest_y);
            }
            player->attacks[j].rect_x = dest_x;
        }
    }

    const int count = 3 + battleParams->countMonsters;
    for (int i = 0; i < count; i++) {
        battleParams->sequence[i] = i;
    }
    battleParams->currSequencePos = 0;
    while(!(battleParams->sequence[battleParams->currSequencePos] < 3)) {
        battleParams->currSequencePos = (battleParams->currSequencePos + 1) % count;
    }
    shuffleSequence(battleParams->sequence, count);

    initializeSelectPositions();
    battleDraw(params);
}

void battleOnExit(void *params) {
    free(params);
}

void handlePlayerTurnInit(BattleParams* battleParams, float dt) {
    if (drawAttackButtonAnimation(battleParams, dt)) {
        battleParams->state = PLAYER_TURN;
    }
}

static void clearSelectRect(PlaydateAPI* pd, int x, int y, bool isAttackOption) {
    if (!isAttackOption)
        pd->graphics->fillRect(x, y, 32, 32, kColorWhite);
    else
        pd->graphics->fillRect(x - 1, y - 1, 127, 32, kColorXOR);
}

void handleFightLoop(BattleParams* battleParams) {
    battleParams->currSequencePos++;
    if(battleParams->currSequencePos < 3) {
        battleParams->state = PLAYER_TURN;
    } else {
        battleParams->state = ENEMY_TURN;
    }
}

Player* selectPlayer(BattleParams* battleParams, PlaydateAPI* pd) {
    PDButtons btn_pressed;
    pd->system->getButtonState(NULL, &btn_pressed, NULL);

    if (btn_pressed & kButtonA) {
        for (int i = 0; i < 3; i++) {
            Player* p = battleParams->chars[i];
            if (p && p->fight_x == battleParams->selectX && p->fight_y == battleParams->selectY) {
                return p;
            }
        }
    } 

    int dx = 0, dy = 0;
    if (btn_pressed & kButtonLeft) {
        dx = -1;
    } else if (btn_pressed & kButtonRight) {
        dx = 1;
    } else if (btn_pressed & kButtonUp) {
        dy = 1;
    } else if (btn_pressed & kButtonDown) {
        dy = -1;
    }

    if (!(dx || dy)) {
        return NULL;
    }


    int oldSelectX = selectPositions[battleParams->selectX][battleParams->selectY][0];
    int oldSelectY = selectPositions[battleParams->selectX][battleParams->selectY][1];
    pd->graphics->fillRect(oldSelectX - 12 , oldSelectY + 12, 8, 8, kColorWhite);
    battleParams->selectX = (battleParams->selectX + dx + 3) % 3;
    battleParams->selectY = (battleParams->selectY + dy + 3) % 3;

    int newSelectX = selectPositions[battleParams->selectX][battleParams->selectY][0];
    int newSelectY = selectPositions[battleParams->selectX][battleParams->selectY][1];

    LCDBitmap* s = pd->graphics->getTableBitmap(battleParams->select, 2);
    pd->graphics->drawBitmap(s, newSelectX - 30, newSelectY, kBitmapUnflipped);

    return NULL;
}

Enemy* selectEnemy(BattleParams* battleParams, PlaydateAPI* pd) {

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
        pd->graphics->fillRect(battleParams->selectX - 12 , battleParams->selectY + 12, 8, 8, kColorWhite);
        battleParams->selectX = 0;
        battleParams->selectY = 0;
        battleParams->state = PLAYER_SELECT_P;
        break;

    case DEBUFF_ARMOR:
        break;

    default:
        break;
    }
}

void handlePlayerTurn(BattleParams* battleParams) {
    PlaydateAPI* pd = battleParams->pd;
    PDButtons btn_pressed;
    pd->system->getButtonState(NULL, &btn_pressed, NULL);

    if (btn_pressed & kButtonA) {
        if(battleParams->selectY < 3) {
            for (int i = 0; i < 3; i++) {
                Player* p = battleParams->chars[i];
                if (p && p->fight_x == battleParams->selectX &&
                    p->fight_y == battleParams->selectY) {
                    battleParams->state = PLAYER_MOVE;
                    battleParams->selectP = i;
                    battleParams->activeP = i;
                    drawAttackOptions(battleParams, pd);
                    return;
                }
            }
        } else {
            int a = (battleParams->selectY == 3 ? 2 : 0) + (battleParams->selectX > 2 ? 1 : 0);
            handleAction(battleParams, pd, battleParams->chars[battleParams->activeP]->attacks[a]);
            return;
        }
    } 

    int dx = 0, dy = 0;
    if (btn_pressed & kButtonLeft) {
        dx = (battleParams->selectY < 3) ? -1 : (battleParams->selectX >= 3 ? -2 : -3);
    } else if (btn_pressed & kButtonRight) {
        dx = (battleParams->selectY < 3) ? 1 : ((battleParams->selectX == 1 || battleParams->selectX == 2) ? 2 : 3);
    } else if (btn_pressed & kButtonUp) {
        dy = 1;
    } else if (btn_pressed & kButtonDown) {
        dy = -1;
    }

    if (!(dx || dy)) {
        return;
    }

    int prevX = battleParams->selectX;
    int prevY = battleParams->selectY;
    int prevSelectX = selectPositions[prevX][prevY][0];
    int prevSelectY = selectPositions[prevX][prevY][1];

    if (prevY < 3) {
        pd->graphics->fillRect(prevSelectX - 12 , prevSelectY + 12, 8, 8, kColorWhite);
    } else {
        pd->graphics->fillRect(prevSelectX - 1, prevSelectY - 1, 127, 32, kColorXOR);
    }

    battleParams->selectX = (battleParams->selectX + dx + 5) % 5;
    battleParams->selectY = (battleParams->selectY + dy + 5) % 5;

    int newSelectX = selectPositions[battleParams->selectX][battleParams->selectY][0];
    int newSelectY = selectPositions[battleParams->selectX][battleParams->selectY][1];

    if (battleParams->selectY < 3) {
        LCDBitmap* s = pd->graphics->getTableBitmap(battleParams->select, 2);
        pd->graphics->drawBitmap(s, newSelectX - 30, newSelectY, kBitmapUnflipped);
    } else {
        pd->graphics->fillRect(newSelectX - 1, newSelectY - 1, 127, 32, kColorXOR);
    }
}

void handlePlayerMove(BattleParams* battleParams) {
    PlaydateAPI* pd = battleParams->pd;
    PDButtons btn_pressed;
    pd->system->getButtonState(NULL, &btn_pressed, NULL);

    if (btn_pressed & kButtonA) {
        battleParams->state = PLAYER_TURN;
        return;
    }

    int dx = 0, dy = 0;
    if (btn_pressed & kButtonLeft)      dx = -1;
    else if (btn_pressed & kButtonRight)  dx = 1;
    else if (btn_pressed & kButtonUp)     dy = 1;
    else if (btn_pressed & kButtonDown)   dy = -1;

    if (!dx && !dy) return;

    int newX = (battleParams->selectX + dx + 3) % 3;
    int newY = (battleParams->selectY + dy + 3) % 3;

    for (int i = 0; i < 3; i++) {
        if (i != battleParams->selectP && battleParams->chars[i] &&
            battleParams->chars[i]->fight_x == newX && battleParams->chars[i]->fight_y == newY) {
            return;
        }
    }

    int prevSelectX = selectPositions[battleParams->selectX][battleParams->selectY][0];
    int prevSelectY = selectPositions[battleParams->selectX][battleParams->selectY][1];
    pd->graphics->fillRect(prevSelectX - 1, prevSelectY - 1, 34, 34, kColorWhite);
    pd->graphics->fillRect(prevSelectX - 12 , prevSelectY + 12, 8, 8, kColorWhite);


    battleParams->selectX = newX;
    battleParams->selectY = newY;
    battleParams->chars[battleParams->selectP]->fight_x = newX;
    battleParams->chars[battleParams->selectP]->fight_y = newY;
    LCDBitmap* s = pd->graphics->getTableBitmap(battleParams->select, 2);

    int newSelectX = selectPositions[battleParams->selectX][battleParams->selectY][0];
    int newSelectY = selectPositions[battleParams->selectX][battleParams->selectY][1];
    pd->graphics->drawBitmap(s, newSelectX - 30, newSelectY, kBitmapUnflipped);
}

void battleUpdate(void* params, float dt) {
    BattleParams* battleParams = (BattleParams*)params;
    PlaydateAPI* pd = battleParams->pd;

    battleParams->passed_time += dt;

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
        case PLAYER_SELECT_P:
            Player* p = selectPlayer(battleParams, pd);
            if(p != NULL) {
                drawAttackOptions(battleParams, pd);
                battleParams->state = PLAYER_TURN;
            }
            break;
        case FIGHT_LOOP:
            handleFightLoop(battleParams);
            break;
        case PLAYER_SELECT_E:
            break;
        default:
            break;
    } 

    const int offset = (GRID_CELL_SIZE - 32) / 2;
    for (int i = 0; i < 3; i++) {
        if (battleParams->chars[i]) {
            int selectX = GRID_LEFT_START_X + battleParams->chars[i]->fight_x * 48 +
                          battleParams->chars[i]->fight_y * 12 + offset;
            int selectY = GRID_LEFT_START_Y - battleParams->chars[i]->fight_y * 42 + offset;
            pd->graphics->fillRect(selectX - 1, selectY - 1, 34, 34, kColorWhite);
            drawAnimatedSprite(&battleParams->chars[i]->sprite, pd, selectX, selectY, true,
                               kBitmapUnflipped, dt);
        }
    }
}

void battleDraw(void* params) {
    BattleParams* battleParams = (BattleParams*)params;
    PlaydateAPI* pd = battleParams->pd;

    pd->graphics->clear(kColorWhite);
    drawGridLeft(pd);
    drawGridRight(battleParams, pd);
    drawAttackSequence(battleParams, pd);
}