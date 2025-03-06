#ifndef SCENEMANAGER_H_
#define SCENEMANAGER_H_

#include <stdbool.h>
#include <stddef.h>

#include "pd_api.h"
#include "player.h"
#include "camera.h"

void overworldEnter(void* params);
void overworldExit(void* params);
void overworldUpdate(void* params, float dt);
void overworldDraw(void* params);

void invOnEnter(void* params);
void invOnExit(void* params);
void invUpdate(void* params, float dt);
void invDraw(void* params);

void battleOnEnter(void* params);
void battleOnExit(void* params);
void battleUpdate(void* params, float dt);
void battleDraw(void* params);

typedef enum {
    MENU,
    OVERWORLD,
    INVENTORY,
    BATTLE
} Scenetypes;

typedef struct {
    void (*OnSceneEnter)(void* params);
    void (*OnSceneExit)(void* params);
    void (*UpdateScene)(void* params, float dt);
    void (*DrawScene)(void* params);
    Scenetypes type;
    void* data;
} Scenemanager;

struct World;

typedef struct {
    PlaydateAPI* pd;
    Scenemanager* manager;
    struct World* world;
    Player* player;
    Camera camera;
    LCDFont* font;
    LCDBitmap* select;
    bool check_info;
    int selectX, selectY;
} OverworldParams;

typedef struct {
    PlaydateAPI* pd;
    float selectedItem;
    Inventory* inv;
} InventoryParams;

typedef struct {
    Player* chars[3];
    PlaydateAPI* pd;
    int selectX, selectY;
    LCDBitmapTable* select;
    LCDBitmapTable* monsters;
} BattleParams;

void changeScene(Scenemanager *manager, Scenetypes type, void* sceneParams);
void updateScene(Scenemanager manager, float dt, PlaydateAPI* pd);
void drawScene(Scenemanager manager, PlaydateAPI* pd);

#endif