#ifndef FIGHT_H_
#define FIGHT_H_

#define TARGET_EXTENSION 1

#include "pd_api.h"
#include "player.h"
#include "enemy.h"

#define GRID_CELL_SIZE 36

typedef struct {
    int x, y;
} Unit;

typedef struct {
    Player* chars[3];
    PlaydateAPI* pd;
    int selectX, selectY;
    LCDBitmapTable* select;
    LCDBitmapTable* monsters;
    SamplePlayer* sampleplayer;
} BattleParams;

void battleOnEnter(void *params);
void battleOnExit(void* params);
void battleUpdate(void* params, float dt);
void battleDraw(void* params);

#endif