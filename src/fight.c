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
    const float speed = 150.0f;

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

void drawPlayerMenu(BattleParams* battleParams, PlaydateAPI* pd) {
    const char* options[4] = {"Show Infos", "Attack", "Escape", "Move Player"};

    for (int i = 0; i < 4; i++) {
        int x = (i % 2) ? 255 : 20;
        int y = (i / 2) ? 200 : 160;
        pd->graphics->drawRect(x, y, 125, 30, kColorBlack);
        pd->graphics->drawText(options[i], strlen(options[i]), kASCIIEncoding, x + 5, y + 7);
        if(i == battleParams->menuIndex) {
            pd->graphics->fillRect(x - 1, y - 1, 127, 32, kColorXOR);
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
                player->attacks[j].rect_y = (float)(dest_y + 80 + 25 * j);
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

void handlePlayerMenu(BattleParams* battleParams, float dt) {
    PlaydateAPI* pd = battleParams->pd;
    PDButtons btn_pressed;
    pd->system->getButtonState(NULL, &btn_pressed, NULL);

    const char* options[4] = {"Show Infos", "Attack", "Escape", "Move Player"};
    const int positions[4][2] = {
        {20, 160},
        {255, 160},
        {20, 200},
        {255, 200}
    };

    if (btn_pressed & kButtonA || battleParams->exit_menu) {
        pd->system->logToConsole("Test");
        if (battleParams->exit_menu) {
            pd->system->logToConsole("Exiting menu");
            for (int i = 0; i < 4; i++) {
                int x = positions[i][0];
                int y = positions[i][1] + (int)(battleParams->menu_offset);
                pd->graphics->fillRect(x - 1, y - 1, 127, 32, kColorWhite);
            }
            battleParams->menu_offset += dt * (float)(150.0);
            if (battleParams->menu_offset >= 240) {
                switch (battleParams->menuIndex) {
                    case 0:
                        battleParams->state = PLAYER_SELECT_INFO;
                        break;
                    case 1:
                        battleParams->state = PLAYER_ATTACK_SELECTION_ANIMATION;
                        break;
                    case 2:
                        battleParams->state = BATTLE_ESCAPE;
                        break;
                    case 3:
                        battleParams->state = PLAYER_MOVE;
                        break;
                }
                return;
            } 
            for (int i = 0; i < 4; i++) {
                int x = positions[i][0];
                int y = positions[i][1] + (int)(battleParams->menu_offset);
                pd->graphics->drawRect(x, y, 125, 30, kColorBlack);
                pd->graphics->drawText(options[i], strlen(options[i]), kASCIIEncoding, x + 5, y + 7);
                if (i == battleParams->menuIndex) {
                    pd->graphics->fillRect(x - 1, y - 1, 127, 32, kColorXOR);
                }
            }
        } else {
            battleParams->exit_menu = true;
        }
        return;
    }

    bool directionalPressed = (btn_pressed & (kButtonLeft | kButtonRight | kButtonUp | kButtonDown)) != 0;

    if (directionalPressed) {
        int col = battleParams->menuIndex % 2;
        int row = battleParams->menuIndex / 2;

        int x = positions[battleParams->menuIndex][0];
        int y = positions[battleParams->menuIndex][1];
        pd->graphics->fillRect(x - 1, y - 1, 127, 32, kColorXOR);

        if (btn_pressed & kButtonLeft || btn_pressed & kButtonRight) {
            col = 1 - col;
        }
        if (btn_pressed & kButtonUp || btn_pressed & kButtonDown) {
            row = 1 - row;
        }

        battleParams->menuIndex = row * 2 + col;

        x = positions[battleParams->menuIndex][0];
        y = positions[battleParams->menuIndex][1];
        pd->graphics->fillRect(x - 1, y - 1, 127, 32, kColorXOR);
    }
}


Player* selectPlayer(BattleParams* battleParams, PlaydateAPI* pd, bool onSelect) {
    PDButtons btn_pressed;
    pd->system->getButtonState(NULL, &btn_pressed, NULL);

    if ((btn_pressed & kButtonA) || onSelect) {
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

void handlePlayerSelectInfo(BattleParams* battleParams, float dt) {
    PlaydateAPI* pd = battleParams->pd;

    PDButtons btn_pressed;
    pd->system->getButtonState(NULL, &btn_pressed, NULL);
    
    Player* p = selectPlayer(battleParams, pd, true);
    
    if (p != NULL && (btn_pressed & kButtonA)) {
        char* s;
        pd->system->formatString(&s, "Name: %s, HP: %d", p->name, p->health);
        pd->graphics->drawText(s, strlen(s), kASCIIEncoding, 20, 200); 
    } else if(btn_pressed & kButtonB) {
        battleParams->state = PLAYER_MENU;
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
        pd->graphics->fillRect(battleParams->selectX - 12 , battleParams->selectY + 12, 8, 8, kColorWhite);
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
    drawAttackOptions(battleParams, battleParams->pd);

    PDButtons btn_pressed;
    battleParams->pd->system->getButtonState(NULL, &btn_pressed, NULL);

    if (btn_pressed & kButtonLeft) {
        battleParams->menuIndex = (battleParams->menuIndex + 3) % 4;
    } else if (btn_pressed & kButtonRight) {
        battleParams->menuIndex = (battleParams->menuIndex + 1) % 4;
    }

    if (btn_pressed & kButtonA) {
        Player* p = battleParams->chars[battleParams->activeP];
        handleAction(battleParams, battleParams->pd, p->attacks[battleParams->menuIndex]);
    } else if(btn_pressed & kButtonB) {
        battleParams->state = PLAYER_MENU;
    }
}

void handleFightLoop(BattleParams* battleParams) {
    battleParams->currSequencePos++;
    if(battleParams->currSequencePos < 3) {
        battleParams->state = PLAYER_MENU;
    } else {
        battleParams->state = ENEMY_TURN;
    }
}

void handleBattleEscape(BattleParams* battleParams) {
    battleParams->state = PLAYER_MENU;
}


Enemy* selectEnemy(BattleParams* battleParams, PlaydateAPI* pd) {
    return NULL;
}

void handlePlayerMove(BattleParams* battleParams) {
    PlaydateAPI* pd = battleParams->pd;
    PDButtons btn_pressed;
    pd->system->getButtonState(NULL, &btn_pressed, NULL);

    if (btn_pressed & kButtonA) {
        battleParams->state = PLAYER_MENU;
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
            battleParams->menuIndex = 0;
            battleParams->menu_offset = 0.0;
            battleParams->exit_menu = false;
            battleParams->state = PLAYER_MENU;
            drawPlayerMenu(battleParams, pd);
            break;

        case PLAYER_MENU:
            handlePlayerMenu(battleParams, dt);
            break;

        case PLAYER_SELECT_INFO:
            handlePlayerSelectInfo(battleParams, dt);
            break;

        case PLAYER_ATTACK_SELECTION_ANIMATION:
            if(drawAttackButtonAnimation(battleParams, dt)) {
                battleParams->state = PLAYER_ATTACK_SELECTION;
            }
            break;

        case PLAYER_ATTACK_SELECTION:
            handlePlayerAttackSelection(battleParams, dt);

        case PLAYER_MOVE:
            handlePlayerMove(battleParams);
            break;

        case PLAYER_SELECT_PLAYER: {
                Player* p = selectPlayer(battleParams, pd, false);
                if(p != NULL) {
                    drawAttackOptions(battleParams, pd);
                    battleParams->state = FIGHT_LOOP;
                }
                break;
            }
        case FIGHT_LOOP:
            handleFightLoop(battleParams);
            break;

        case PLAYER_SELECT_ENEMY:
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