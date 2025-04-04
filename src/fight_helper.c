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
        pd->system->logToConsole("X: %d, Y:%d", battleParams->selectX, battleParams->selectY);
    
        int newSelectX = selectPositions[battleParams->selectX][battleParams->selectY][0];
        int newSelectY = selectPositions[battleParams->selectX][battleParams->selectY][1];
        LCDBitmap* s = pd->graphics->getTableBitmap(battleParams->selectorIcons, 2);
        pd->graphics->drawBitmap(s, newSelectX - 30, newSelectY, kBitmapUnflipped);
        pd->graphics->fillRect(0, 155, 400, 85, kColorWhite);
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
        // if(e->hp <= 0) continue;
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
        pd->system->logToConsole("X: %d, Y:%d", battleParams->selectX, battleParams->selectY);
    
        int newSelectX = selectPositions[battleParams->selectX][battleParams->selectY][0];
        int newSelectY = selectPositions[battleParams->selectX][battleParams->selectY][1];
        LCDBitmap* s = pd->graphics->getTableBitmap(battleParams->selectorIcons, 2);
        pd->graphics->drawBitmap(s, newSelectX - 30, newSelectY, kBitmapUnflipped);
        pd->graphics->fillRect(0, 155, 400, 85, kColorWhite);
    }

    if ((btn_pressed & kButtonA) || onSelect) {
        for (int i = 3; i < NUM_PLAYERS + battleParams->enemyCount; i++) {
            Enemy* e = &battleParams->enemies[i];
            if (e && e->fight_x == battleParams->selectX && e->fight_y == battleParams->selectY) {
                return e;
            }
        }
    }
    return NULL;
}