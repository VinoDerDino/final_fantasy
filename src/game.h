#ifndef GAME_H_
#define GAME_H_

#include "pd_api.h"
#include "player.h"
#include "item.h"
#include "inventory.h"
#include "scenemanager.h"

typedef struct {
    Player chars[3];
    Scenemanager scenemanager;
    ItemList itemlist;
    LCDBitmapTable* itemtable;
    LCDBitmapTable* tiletable;
    float lastFrameTime;
    World world;
} Game;

#endif