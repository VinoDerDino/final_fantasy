#include "fight.h"

int attackEnemy(Enemy *e, Attack attack) {
    int totalDamage = attack.dmg;
    if(attack.pattern == e->resistenceType) {
        totalDamage -= e->resistence;
    }

    if(attack.type != PIERCING_ATTACK) {
        totalDamage -= e->defense;
    }
    e->hp -= totalDamage;
    if(e->hp <= 0) {
        e->isAlive = false;
    }
    return totalDamage;
}

void shuffleSequence(int* sequence, int count) {
    for (int i = count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = sequence[i];
        sequence[i] = sequence[j];
        sequence[j] = temp;
    }
}

void getDirection(PDButtons btn_pressed, int* dx, int* dy) {
    *dx = 0;
    *dy = 0;
    if (btn_pressed & kButtonLeft)  *dx = -1;
    else if (btn_pressed & kButtonRight) *dx = 1;
    if (btn_pressed & kButtonUp)    *dy = 1;
    else if (btn_pressed & kButtonDown)  *dy = -1;
}

Player* selectPlayer(BattleParams* battleParams, PlaydateAPI* pd, bool onSelect) {
    PDButtons btn_pressed;
    pd->system->getButtonState(NULL, &btn_pressed, NULL);

    int dx = 0, dy = 0;
    getDirection(btn_pressed, &dx, &dy);

    if (dy) {
        int oldSelectX = selectPositions[battleParams->selectX][battleParams->selectY][0];
        int oldSelectY = selectPositions[battleParams->selectX][battleParams->selectY][1];
        pd->graphics->fillRect(oldSelectX - 12, oldSelectY + 12, 8, 8, kColorWhite);
        battleParams->selectX = 0;
        battleParams->selectY = (battleParams->selectY + dy + 3) % 3;
    
        int newSelectX = selectPositions[battleParams->selectX][battleParams->selectY][0];
        int newSelectY = selectPositions[battleParams->selectX][battleParams->selectY][1];
        LCDBitmap* s = pd->graphics->getTableBitmap(battleParams->selectorIcons, 2);
        pd->graphics->drawBitmap(s, newSelectX - 30, newSelectY, kBitmapUnflipped);
        clearInfoArea(pd);
    }

    if ((btn_pressed & kButtonA) || onSelect) {
        for (int i = 0; i < NUM_PLAYERS; i++) {
            Player* p = battleParams->players[i];
            if (p && p->fight_y == battleParams->selectY) {
                return p;
            }
        }
    }
    return NULL;
}

Enemy* getEnemyAtPos(BattleParams* battleParams, int selectX, int selectY) {

    for(int i = 0; i < battleParams->enemyCount; i++) {
        Enemy* e = &battleParams->enemies[i];
        if(!e->isAlive) continue;
        if(e->fight_x == selectX && e->fight_y == selectY) {
            return e;
        }
    }
    return NULL;
}

Enemy* selectEnemy(BattleParams* battleParams, PlaydateAPI* pd, bool onSelect) {
    PDButtons btn_pressed;
    pd->system->getButtonState(NULL, &btn_pressed, NULL);

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
        clearInfoArea(pd);
    }

    if ((btn_pressed & kButtonA) || onSelect) {
        for (int i = 3; i < NUM_PLAYERS + battleParams->enemyCount; i++) {
            Enemy* e = &battleParams->enemies[i];
            if (e && e->isAlive && e->fight_x == battleParams->selectX && e->fight_y == battleParams->selectY) {
                return e;
            }
        }
    }
    return NULL;
}

char *substring(const char* str, size_t n) {
    size_t len = strlen(str);
    if(n > len) n = len;

    char* result = malloc(n + 1);
    if(result == NULL) return NULL;

    strncpy(result, str, n);
    result[n] = '\0';

    return result;
}

void clearInfoArea(PlaydateAPI* pd) {
    pd->graphics->fillRect(4, 165, 392, 72, kColorWhite);
    pd->graphics->fillRect(10, 158, 380, 79, kColorWhite);
}

void getGridPosition(int fight_x, int fight_y, int* xPos, int* yPos) {
    *xPos = selectPositions[fight_x][fight_y][0];
    *yPos = selectPositions[fight_x][fight_y][1];
}

void playAButtonSound(BattleParams *battleParams) {
    battleParams->pd->sound->synth->playNote(battleParams->synth, 400.f, 1.f, 0.2f, 0);
}

void playBButtonSound(BattleParams *battleParams) {
    battleParams->pd->sound->synth->playNote(battleParams->synth, 300.f, 1.f, 0.2f, 0);
}