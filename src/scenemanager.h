#ifndef SCENEMANAGER_H_
#define SCENEMANAGER_H_

#include <stdbool.h>

#include "pd_api.h"
#include "player.h"
#include "enemy.h"
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
    PLAYER_TURN_INIT,
    PLAYER_MENU,
    PLAYER_MENU_EXIT,
    PLAYER_SELECT_INFO,
    PLAYER_ATTACK_SELECTION_ANIMATION,
    PLAYER_ATTACK_SELECTION,
    PLAYER_MOVE,
    PLAYER_SELECT_PLAYER,
    FIGHT_LOOP,
    PLAYER_SELECT_ENEMY,
    ENEMY_TURN,
    BATTLE_ESCAPE
} BattleStates;

typedef enum {
    PENDING = -1,
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
    Scenetypes pendingSceneChange;
    void* pendingData;
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
    LCDBitmapTable* select;
    LCDBitmapTable* monsters;
    int selectX, selectY, selectP, activeP, menuIndex;
    int countMonsters;
    Enemy enemies[3];
    int sequence[6];
    int currSequencePos;
    BattleStates state;
    float passed_time;
    bool exit_menu;
    float menu_offset;
} BattleParams;

void changeScene(Scenemanager *manager, Scenetypes type, void* sceneParams);
void updateScene(Scenemanager* manager, float dt, PlaydateAPI* pd);
void drawScene(Scenemanager* manager, PlaydateAPI* pd);

#endif